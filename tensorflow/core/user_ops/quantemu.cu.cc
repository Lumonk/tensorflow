#ifdef GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "quantemu.h"
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

using namespace tensorflow;
using GPUDevice = Eigen::GpuDevice;

#define CUBLOCK_SIZE 1024 

__device__ float atomicMaxf(float* address, float val)
{
    int *address_as_int =(int*)address;
    int old = *address_as_int, assumed;
    while (val > __int_as_float(old)) {
        assumed = old;
        old = atomicCAS(address_as_int, assumed,
                        __float_as_int(val));
        }
    return __int_as_float(old);
}

__global__ void max_reduce(const float* const d_array, float* d_max, const int block_offset, const int block_size)
{
    volatile __shared__ float shared[CUBLOCK_SIZE]; 

    int tid = threadIdx.x;
    int gid = (blockDim.x * blockIdx.x) + tid + block_offset; 
    shared[tid] = 0; 
    size_t elements = block_offset+ block_size; 

    while (gid < elements) {
        shared[tid] = fmaxf(fabsf(shared[tid]), fabsf(d_array[gid]));
        gid += gridDim.x*blockDim.x;
        }
    __syncthreads();
    gid = (blockDim.x * blockIdx.x) + tid + block_offset;  // 1
    for (unsigned int s=blockDim.x/2; s>0; s>>=1) 
    {
        if (tid < s && gid < elements)
            shared[tid] = fmaxf(fabsf(shared[tid]), fabsf(shared[tid + s]));
        __syncthreads();
    }

    if (tid == 0)
      atomicMaxf(d_max, shared[0]);
}

template <typename T>
__global__ void QuantEmuCudaKernel(int mbits, T *absmax, int rmode, const int block_offset, const int block_size, const T* in, T* out) {

  int quant_max; 
  T sfquant;
  T sfdequant;
//  T sfquant_br; 

  //quant_max = pow(2, mbits-1) - 1;
  quant_max = (int)((0x1 << (mbits-1)) - 1);
  sfquant = (T)(quant_max / *absmax);
  sfdequant = (T)1.0/sfquant;
//  sfquant_br = sfquant * 4.0;  /* quantize to nbits + 2, we need them for rounding */ 

  int tid = threadIdx.x;
  int gid = (blockDim.x * blockIdx.x) + tid + block_offset;
  int size = block_offset + block_size; 
  T fabsmax = *absmax; 
 
//  for (int gid = (blockIdx.x * blockDim.x) + threadIdx.x + block_offset; gid < size; gid += blockDim.x * gridDim.x) {
  for (gid; gid < size; gid += blockDim.x*gridDim.x) {
      T inval = in[gid];
      /* saturate anything larger than fmax_val to fmax_val */
      fminf (inval, fabsmax); 
      fmaxf (inval, -fabsmax);
      int ival = (int)(inval * sfquant);
#if 0
      int rbias = ((int)(inval * sfquant_br)) & 0x3;
      //if (in[i] == 0.0) { ival = 0; rbias = 0; }
      int negative = (ival < 0);
      /* disable sign bit for rounding */
      if(negative) ival = 0 - ival;
      ival += ((rmode==BIASED) & (rbias > 0));
      ival += ((rmode==NEAREST) & (rbias > 1));
      /* saturate after rounding */
      if (ival > quant_max) ival = quant_max;
      /* restore sign */
      if(negative) ival = 0 - ival;
#endif 
      out[gid] = ival * sfdequant;
  }
}
#if 0
template <typename T>
__global__ void QuantEmuFunctorKernel (int mbits, int rmode, int size, const T* in, T* out) {
   __syncthreads();
     T d_absmax;
     max_reduce(in, &d_absmax, 0, size); 
   __syncthreads();
     QuantEmuCudaKernel<T> (mbits, &d_absmax, rmode, 0, size, in, out);
   __syncthreads();
};
#endif 
template <typename T>
__global__ void QuantEmuLowpCudaKernel(int mbits, int exp_bits, int rmode, const int size, const T* in, T* out) {

  int non_mant_bits = exp_bits + 1; /* exponent + sign */
  int shift = 10 - (mbits - non_mant_bits);
//  unsigned short lowpfp_mask = (unsigned short)(0xFFFF << shift);

  for (int gid = (blockIdx.x * blockDim.x) + threadIdx.x; gid < size; gid += blockDim.x * gridDim.x) {
      T inval = in[gid];
      __half  hval = __float2half(inval); 
      //hval = (hval & lowpfp_mask); 
      T outval = __half2float(hval);
      out[gid] = outval;
  }
}


/* Define the GPU implementation that launches the CUDA kernel. */
template <typename T>
struct QuantEmuFunctor<GPUDevice, T> { 
  void operator()(const GPUDevice& d, int mbits, int rmode, int size, const T* in, T* out) {
//    std::cout << " Inside the QuantEmuFunctor GPU version "<< size  << std::endl; 
    int block = CUBLOCK_SIZE; 
    int grid = (size + (CUBLOCK_SIZE -1))/CUBLOCK_SIZE;  
#if 1
    T *d_absmax;
    cudaMalloc((void**) &d_absmax, sizeof(T));
    max_reduce<<<grid, block, 0, d.stream()>>>(in, d_absmax, 0, size); 
//    cudaStreamSynchronize(d.stream());
    QuantEmuCudaKernel<T> <<<grid, block, 0, d.stream()>>>(mbits, d_absmax, rmode, 0, size, in, out);
//    cudaStreamSynchronize(d.stream());
    cudaFree(d_absmax);
#else 
    QuantEmuFunctorKernel<T><<<grid, block, 0, d.stream()>>> (mbits, rmode, size, in, out); 
    cudaStreamSynchronize(d.stream());
#endif 
  }
};
template struct QuantEmuFunctor<GPUDevice, float>;

template <typename T>
struct BlockC_QuantEmuFunctor<GPUDevice, T> {
  void operator()(const GPUDevice& d, int mbits, int *dims , int block_size, int rmode, const T *in, T *out) {
#if 10
//    std::cout << " Inside the BlockCQuantEmuFunctorGPU version, block_size: " << block_size << std::endl; 

    int c_blocks =  dims[3]/block_size; 
    if ((dims[3]%block_size) || (dims[3] < block_size)) { c_blocks = 1; block_size = dims[3];}
#pragma omp parallel for collapse(3) 
    for (int d0 = 0; d0 < dims[0]; d0++) {
      for (int d1 = 0; d1 < dims[1]; d1++) {
        for (int d2 = 0; d2 < dims[2]; d2++) {
          for (int d3 = 0; d3 < c_blocks; d3++) {
	    const int tensor_offset = d0*dims[1]*dims[2]*dims[3] + d1*dims[2]*dims[3] + d2*dims[3]; 
            const int block_offset = tensor_offset + d3*block_size; 

            int block = CUBLOCK_SIZE; //(CUBLOCK_SIZE > block_size)?block_size:CUBLOCK_SIZE; 
            int grid = (block_size + (block -1))/block;  
            const T *input_flat = in; 
            T *output_flat = out; 

            T *d_absmax;
            cudaMalloc((void**) &d_absmax, sizeof(T));
            max_reduce<<<grid, block, 0, d.stream()>>>(input_flat, d_absmax, block_offset, block_size); 
//            cudaStreamSynchronize(d.stream());
            QuantEmuCudaKernel<T> <<<grid, block, 0, d.stream()>>>(mbits, d_absmax, rmode, block_offset, block_size, input_flat, output_flat);
//            cudaStreamSynchronize(d.stream());
//            cudaDeviceSynchronize();
            cudaFree(d_absmax);
	  }
        }
      }
    }
#endif 
  }
};
template struct BlockC_QuantEmuFunctor<GPUDevice, float>;

template <typename T>
struct BlockCHW_QuantEmuFunctor<GPUDevice, T> {
  void operator()(const GPUDevice& d, int mbits, int *dims, int cblock_size, int rmode, const T *in, T *out) {
//    std::cout << " Inside the BlockQuantEmuFunctorGPU version " << std::endl; 
#if 10
    int chw_blocks =  dims[1]/cblock_size; 
    if ((dims[1]%cblock_size) || (dims[1] < cblock_size)) { chw_blocks = 1; cblock_size = dims[1];}
    int block_size = cblock_size*dims[2]*dims[3];

#pragma omp parallel for collapse(2) 
    for (int d0 = 0; d0 < dims[0]; d0++) {
      for (int d1 = 0; d1 < chw_blocks; d1++) {
	int tensor_offset = d0*dims[1]*dims[2]*dims[3];
        int block_offset = tensor_offset + d1*block_size; 

        int block = CUBLOCK_SIZE; //(CUBLOCK_SIZE > block_size)?block_size:CUBLOCK_SIZE; 
        int grid = (block_size + (block -1))/block;  
        const T *input_flat = in; 
        T *output_flat = out; 

        T *d_absmax;
        cudaMalloc((void**) &d_absmax, sizeof(T));
        max_reduce<<<grid, block, 0, d.stream()>>>(input_flat, d_absmax, block_offset, block_size); 
//        cudaStreamSynchronize(d.stream());
        QuantEmuCudaKernel<T> <<<grid, block, 0, d.stream()>>>(mbits, d_absmax, rmode, block_offset, block_size, input_flat, output_flat);
//        cudaStreamSynchronize(d.stream());
//        cudaDeviceSynchronize();
        cudaFree(d_absmax);
      }
    }
#endif 
  }
};
template struct BlockCHW_QuantEmuFunctor<GPUDevice, float>;


#define  CUTHREADS 32 
template <typename T>
struct LowpFloatQuantEmuFunctor <GPUDevice, T> {
  void operator()(const GPUDevice& d, int mbits, int exp_bits, int rmode, int size, const T* in, T* out) {
//    std::cout << " Inside the LowpFloatQuantEmuFunctor GPU version "<< size  << std::endl; 
    int block = CUBLOCK_SIZE; 
    int grid = ( size + (CUBLOCK_SIZE-1))/CUBLOCK_SIZE; 
    QuantEmuLowpCudaKernel<T> <<<grid, block, 0, d.stream()>>>(mbits, exp_bits, rmode, size, in, out); 
//    cudaStreamSynchronize(d.stream());
//    cudaDeviceSynchronize();
  }
};
template struct LowpFloatQuantEmuFunctor<GPUDevice, float>;

#endif  // GOOGLE_CUDA
