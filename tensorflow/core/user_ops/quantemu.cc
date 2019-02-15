#include "quantemu.h"
#include "tensorflow/core/framework/shape_inference.h"
#include <assert.h>

using namespace tensorflow;  

#include <immintrin.h> 
#include <omp.h> 
//#include "posit_impl.cc" 
#include "posit_impl.h" 
//#include "posit_types.h" 
  
using CPUDevice = Eigen::ThreadPoolDevice;
using GPUDevice = Eigen::GpuDevice;

REGISTER_OP("QuantizeEmu")
    //.Attr("T: {float, Eigen::half, int32} = DT_FLOAT")
    .Attr("T: type")
    .Input("to_quantize: T")
    .Output("quantized: T")
    .Attr("data_format: {'unknown', 'channels_first', 'channels_last'}")
    .Attr("allocate_copy: int = 0") 
    .Attr("data_type: int = 0")
    .Attr("precision: int = 23")
    .Attr("exponent_bits: int = 5")
    .Attr("channel_blocking_type: int = 0")
    .Attr("channels_per_block: int = 0")
    .Attr("round_mode: int = 0")
    .Attr("quantize_gradients: int = 0")
    .SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c) {
      c->set_output(0, c->input(0));
      return Status::OK(); 
    });


#define ROUNDING_CTRL (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC)
#ifdef AVX512F
__m512 _mm512_cvtpsph_ps(__m512 reg)
{
  __m256i hreg  = _mm512_cvtps_ph(reg, ROUNDING_CTRL);
  return  _mm512_cvtph_ps (hreg);
}
#elif AVX 
__m256 _mm256_cvtpsph_ps(__m256 reg)
{
  __m128i hreg  = _mm256_cvtps_ph(reg, ROUNDING_CTRL);
  return  _mm256_cvtph_ps (hreg);
}
#endif 

/* CPU specialization of actual computation. */
template <typename T>
struct QuantEmuFunctor<CPUDevice, T> {
  void operator()(const CPUDevice& d, int unsigned_data, int mbits, int rmode, int size, T* in, T* out) {

    //std::cout << " *** Inside the QuantEmuFunctor CPU version "<< size  << std::endl; 
    T absmax = 0.0; 
    #pragma omp parallel for reduction(max: absmax)
    for (int i=0; i < size; i++) if (fabs(in[i]) > absmax) absmax = fabs(in[i]);  

    int quant_max = pow(2, mbits-1) - 1;
    if (unsigned_data == 1) quant_max = pow(2, mbits) - 1;

    T sfquant = (T)(quant_max/absmax);
    T sfdequant = (T)1.0/sfquant;
    T sfquant_br = sfquant * 4.0;  /* quantize to nbits + 2, we need them for rounding */

//    __m256 vsfquant    = _mm256_set1_ps (sfquant);
//    __m256 vsfdequant    = _mm256_set1_ps (sfdequant);

    if ( size%16 == 0 ) 
    {
#ifdef AVX512F
      __m512 vmaxpos     = _mm512_set1_ps (absmax);
      __m512 vmaxneg     = _mm512_set1_ps (-absmax);
      #pragma omp parallel for 
      for (int i = 0; i < size; i+=16) {
        __m512 vinp = _mm512_load_ps (&in[i]);
        /* saturate to absmax value */ 
        vinp = _mm512_min_ps (vinp, vmaxpos); 
        vinp = _mm512_max_ps (vinp, vmaxneg); 

        /* quantize */ 
        //__m512i vint    = _mm512_cvtps_epi32 (_mm512_mul_ps (vinp, vsfquant));
        /* quantize and round to nearest even */ 
        __m512i vint    = _mm512_rint_ps(_mm512_mul_ps (vinp, vsfquant));
        /* dequantize */ 
        __m512 vout = _mm512_mul_ps ( _mm512_cvtepi32_ps (vint), vsfdequant); 

        _mm512_store_ps (&out[i], vout); 
      }
#elif AVX
      __m256 vmaxpos     = _mm256_set1_ps (absmax);
      __m256 vmaxneg     = _mm256_set1_ps (-absmax);
#if 0
      __m256 vsfquant_br = _mm256_set1_ps (sfquant_br);
      __m256i vrmask     = _mm256_set1_epi32 (0x3);
      __m256i vposone    = _mm256_set1_epi32 (1);
      __m256i vzero      = _mm256_set1_epi32 (0);
      __m256i vnegone    = _mm256_set1_epi32 (-1);

      __m256i vrthreshold  = _mm256_set1_epi32 (0x0); /* BIASED */
      if (rmode==NEAREST) vrthreshold  = _mm256_set1_epi32 (0x0); /* NEAREST */
#endif 
      #pragma omp parallel for 
      for (int i = 0; i < size; i+=8) {
        __m256 vinp = _mm256_load_ps (&in[i]);
        vinp = _mm256_min_ps (vinp, vmaxpos); 
        vinp = _mm256_max_ps (vinp, vmaxneg); 
        /* quantize */ 
        __m256i vint    = _mm256_cvtps_epi32 (_mm256_mul_ps (vinp, vsfquant));
#if 0      
        __m256i vint_br =_mm256_and_si256 (_mm256_cvtps_epi32 (_mm256_mul_ps (vinp, vsfquant_br)), vrmask);
        /* flip sign and round */ 
        __mmask8 rmask  = _mm256_cmp_epi32_mask (vint_br, vrthreshold, _MM_CMPINT_NLE);
        __m256i vsignflip = _mm256_mask_mov_epi32 (vposone, _mm256_cmp_epi32_mask (vint, vzero, _MM_CMPINT_LT), vnegone ); 
        __m256i vuint = _mm256_mul_epi32 (vint, vsignflip); 
        vuint = _mm256_mask_add_epi32 (vuint, rmask, vuint, vposone);
        vint = _mm256_mul_epi32 (vuint, vsignflip); 
#endif 
        /* dequantize */ 
        __m256 vout = _mm256_mul_ps ( _mm256_cvtepi32_ps (vint), vsfdequant); 
        _mm256_store_ps (&out[i], vout); 
      }
#endif 
    } else {
 
      #pragma omp parallel for 
      for (int i = 0; i < size; ++i) {
        T inval = in[i];

        /* saturate anything larger than fmax_val to fmax_val */
        if (inval > absmax && inval > 0 ) inval = absmax;
        if (fabs(inval) > absmax && inval < 0 ) inval = -absmax;

        int ival = (int)(inval * sfquant);
        int rbias = ((int)(inval * sfquant_br)) & 0x3;
        //if (in[i] == 0.0) { ival = 0; rbias = 0; }
        int negative = (ival < 0);
        /* disable sign bit for rounding */
        if(negative) ival = 0 - ival;
//        ival += ((rmode==BIASED) & (rbias > 0));
//        ival += ((rmode==NEAREST) & (rbias > 1));
        /* saturate after rounding */
        if (ival > quant_max) ival = quant_max;
        /* restore sign */
        if(negative) ival = 0 - ival;

        out[i] = ival * sfdequant;
      }
    }
  }
};

/* CPU specialization of actual computation. */
template <typename T>
struct BlockC_QuantEmuFunctor<CPUDevice, T> {
  void operator()(const CPUDevice& d, int unsigned_data, int mbits, int *dims, 
	int block_size, int rmode, T *in, T *out) {

    T *input_flat = in; 
    T *output_flat = out; 

    int c_blocks =  dims[3]/block_size; 
    if ((dims[3]%block_size) || (dims[3] < block_size)) { c_blocks = 1; block_size = dims[3];}

    for (int d0 = 0; d0 < dims[0]; d0++) {
      for (int d1 = 0; d1 < dims[1]; d1++) {
        for (int d2 = 0; d2 < dims[2]; d2++) {
          for (int d3 = 0; d3 < c_blocks; d3++) {
	    int tensor_offset = d0*dims[1]*dims[2]*dims[3] + d1*dims[2]*dims[3] + d2*dims[3]; 
            int block_offset = d3*block_size; 
  	    T *input_block = &input_flat[tensor_offset + block_offset]; 
  	    T *output_block = &output_flat[tensor_offset + block_offset]; 

            QuantEmuFunctor<CPUDevice, T>()(
                d,
                unsigned_data,
                mbits, 	
	        rmode,
                block_size,  
                input_block,
                output_block);
	  }
        }
      }
    }
  }
};

template <typename T>
struct BlockCHW_QuantEmuFunctor<CPUDevice, T> {
  void operator()(const CPUDevice& d, int unsigned_data, int mbits, int *dims, 
		int cblock, int rmode, T *in, T *out) {

    int chw_blocks =  dims[1]/cblock; 
    if ((dims[1]%cblock) || (dims[1] < cblock)) { chw_blocks = 1; cblock = dims[1];}
    int block_size = cblock*dims[2]*dims[3];

    T *input_flat = in; 
    T *output_flat = out; 

    for (int d0 = 0; d0 < dims[0]; d0++) {
      for (int d1 = 0; d1 < chw_blocks; d1++) {
	int tensor_offset = d0*dims[1]*dims[2]*dims[3];
        int block_offset = d1*block_size; 
  	T *input_block = &input_flat[tensor_offset + block_offset]; 
  	T *output_block = &output_flat[tensor_offset + block_offset]; 

        QuantEmuFunctor<CPUDevice, T>()(
                d,
                unsigned_data,
                mbits, 	
	        rmode,
                block_size,  
                input_block,
                output_block);
      }
    }
  }
};

template <typename T>
struct LowpFloatQuantEmuFunctor <CPUDevice, T> {
  void operator()(const CPUDevice& d, int mbits, int exp_bits, int rmode, int size, const T* in, T* out) {

    int non_mant_bits = exp_bits + 1; /* exponent + sign */
    //std::cout << "LowpFloatQuantEmuFunctor, non_mant_bits: " << non_mant_bits << ", mbits: " << mbits  << ", size: " << size << std::endl;
    if (mbits <= non_mant_bits) { printf("LPFP size cannot be <=5\n"); exit (0); }
    int shift = 10 - (mbits - non_mant_bits);
    unsigned short lowpfp_mask = (unsigned short)(0xFFFF << shift);
    __m128i lpmask = _mm_set1_epi16 (lowpfp_mask);

    #pragma omp parallel for 
    for (int a= 0; a < size; a+=8) {
       __m256 v32 = _mm256_load_ps((float*)&in[a]);
       __m128i v16l = _mm256_cvtps_ph(v32, ROUNDING_CTRL);
       v16l = _mm_and_si128 (v16l, lpmask);
       v32 = _mm256_cvtph_ps (v16l);
       _mm256_store_ps((float*)&out[a], v32);
    }
  }
};

template <typename T>
struct Log2QuantEmuFunctor <CPUDevice, T> {
  void operator()(const CPUDevice& d, int size, const T* in, T* out) {
     /* to be implemented, refer to CUDA version */ 
  }
};

template <typename T>
struct PositQuantEmuFunctor <CPUDevice, T> {
  void operator()(const CPUDevice& d, int m_bits, int es_bits, int size, const T* in, T* out) {

    #pragma omp parallel for 
    for (int a= 0; a < size; a++) {
      POSIT_UTYPE pval =pack_posit(unpack_float(in[a]), m_bits, es_bits); 
      out[a] = pack_float (unpack_posit(pval, m_bits, es_bits));  
    }
  }
};

template <typename T>
struct BfloatQuantEmuFunctor <CPUDevice, T> {
  void operator()(const CPUDevice& d, int size, const T* in, T* out) {
#if 0
    #pragma omp parallel for 
    for (int a= 0; a < size; a++) {
      POSIT_UTYPE pval =pack_posit(unpack_float(in[a]), m_bits, es_bits); 
      out[a] = pack_float (unpack_posit(pval, m_bits, es_bits));  
    }
#endif 
  }
};


/* OpKernel definition.
 template parameter <T> is the datatype of the tensors.*/
template <typename Device, typename T>
class QuantEmuOp : public OpKernel {
 public:
  explicit QuantEmuOp(OpKernelConstruction* context) : OpKernel(context) {
    /* aquire the OP attributes */ 
    OP_REQUIRES_OK(context, context->GetAttr("data_format", &data_format_));
    OP_REQUIRES_OK(context, context->GetAttr("allocate_copy", &allocate_copy_));
    OP_REQUIRES_OK(context, context->GetAttr("data_type", &lpdata_type_));
    OP_REQUIRES_OK(context, context->GetAttr("precision", &mbits_));
    OP_REQUIRES_OK(context, context->GetAttr("exponent_bits", &exponent_bits_));
    OP_REQUIRES_OK(context, context->GetAttr("channel_blocking_type", &block_type_));
    OP_REQUIRES_OK(context, context->GetAttr("channels_per_block", &block_size_));
    OP_REQUIRES_OK(context, context->GetAttr("round_mode", &round_mode_));
    OP_REQUIRES_OK(context, context->GetAttr("quantize_gradients", &quantize_grad_));

    //std::cout << "data_format : " << data_format_ << ", lpdata_type: " << lpdata_type_ << ", mbits_: " << mbits_ << 
    // ", exponent_bits_: " << exponent_bits_ << ", quantize_grad_: " << quantize_grad_ << ", mbits_grad_ : " << mbits_grad_ << 
    // ", block_type: " << block_type_ << ", block_size: " << block_size_ << ", round_mode: " << round_mode_ << std::endl;
  }

  void Compute(OpKernelContext* context) override {
    /* Grab the input tensor */ 
    const Tensor& input_tensor = context->input(0);
    //const Tensor& nbits = context->input(1);
    //auto mbits_flat = nbits.flat<int32>(); 
    //int mbits = mbits_flat(0);
    int mbits = mbits_;
    Tensor *poutput_tensor;
    Tensor output_tensor;

    if (allocate_copy_ == 1) { 
      /* allocate fresh copy for quantized buffer */ 
      OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor.shape(), &poutput_tensor));
    } else { 
      /* use the same underlying buffer for both input and output tensors */  
      CHECK(output_tensor.CopyFrom(input_tensor, input_tensor.shape()));
      context->set_output(0, output_tensor);
      poutput_tensor = &output_tensor; 
    }

    /* Do the computation. */
    OP_REQUIRES(context, input_tensor.NumElements() <= tensorflow::kint32max,
                errors::InvalidArgument("Too many elements in tensor"));
    OP_REQUIRES(context, input_tensor.dims() <= 4,
                errors::InvalidArgument("Input tensor shape consists of more than 4 dimentions"));

    switch (lpdata_type_) {
      case INT: 
      case UINT: 
      {
        switch (block_type_ ) 
        {
          case NOBLOCK:
          { 
            QuantEmuFunctor<Device, T>()(
              context->eigen_device<Device>(),
              (lpdata_type_ == UINT)?1:0, 
              mbits, 	
	      round_mode_,
              static_cast<int>(input_tensor.NumElements()),
              (T*)input_tensor.flat<T>().data(),
              //output_tensor.flat<T>().data());
              poutput_tensor->flat<T>().data());
          }
          break; 
          case BLOCK_C: 
          {
	    if (data_format_.compare("channels_last")) { 
	      std::cout << "BLOCK_C blocking mode is not suported on " << data_format_ << " data format, use NHWC data_format." << std::endl; exit(0); 
            }
            if ( input_tensor.dims() > 4 ) { std::cout << "quantemu_op does not support tensors with more than 4 dimentions" << std::endl; exit (0); }
            int dims[4] = { 1, 1, 1, 1}; 
            for (int d=0; d < input_tensor.dims(); d++ ) dims[d] = input_tensor.dim_size(d); 

            BlockC_QuantEmuFunctor<Device, T>()(
	      context->eigen_device<Device>(), 
              (lpdata_type_ == UINT)?1:0, 
	      mbits, 
	      dims, 
              block_size_, 
	      round_mode_, 
              (T*)input_tensor.flat<T>().data(),
              //output_tensor.flat<T>().data());
              poutput_tensor->flat<T>().data());
          }
          break; 
          case BLOCK_CHW: 
          {
	    if (data_format_.compare("channels_first")) { 
	       std::cout << "BLOCK_CHW blocking mode is not suported on " << data_format_ << " data format, use NCHW data_format." << std::endl; exit(0); 
            }	
            if ( input_tensor.dims() > 4 ) { std::cout << "quantemu_op does not support tensors with more than 4 dimentions" << std::endl; exit (0); }
            int dims[4] = { 1, 1, 1, 1}; 
            for (int d=0; d < input_tensor.dims(); d++ ) dims[d] = input_tensor.dim_size(d); 
            //std::cout << "CHW blocking : format: " << data_format_ << ", block_size : " << block_size_ << std::endl;

            BlockCHW_QuantEmuFunctor<Device, T>()(
	      context->eigen_device<Device>(), 
              (lpdata_type_ == UINT)?1:0, 
	      mbits, 
	      dims,
              block_size_, 
	      round_mode_, 
              (T*)input_tensor.flat<T>().data(),
              //output_tensor.flat<T>().data());
              poutput_tensor->flat<T>().data());
          }
          break; 
        }
      }
      break; 
      case LOWP_FP: 
      {
	//std::cout << "LowpFP quantization called" << ", mbits : " << mbits <<  std::endl; 
        LowpFloatQuantEmuFunctor<Device, T>()(
          context->eigen_device<Device>(),
          mbits, 	
          exponent_bits_,
	  round_mode_,
          static_cast<int>(input_tensor.NumElements()),
          input_tensor.flat<T>().data(),
          //output_tensor.flat<T>().data());
          poutput_tensor->flat<T>().data());
      }
      break; 
      case LOG2: 
      {
	//std::cout << "Log2 quantization called" << ", mbits : " << mbits <<  std::endl; 
        Log2QuantEmuFunctor<Device, T>()(
          context->eigen_device<Device>(),
          static_cast<int>(input_tensor.NumElements()),
          input_tensor.flat<T>().data(),
          poutput_tensor->flat<T>().data());
      }
      break; 
      case POSIT: 
      {
	//std::cout << "Posit quantization called" << ", mbits : " << mbits <<  std::endl; 
        PositQuantEmuFunctor<Device, T>()(
          context->eigen_device<Device>(),
          mbits, 	
          exponent_bits_,
          static_cast<int>(input_tensor.NumElements()),
          input_tensor.flat<T>().data(),
          poutput_tensor->flat<T>().data());
      }
      break; 
      case BFLOAT: 
      {
	//std::cout << "BFLOAT-16 quantization called" << ", mbits : " << mbits <<  std::endl; 
  	/* standard bfloat16 with RNE, mbits is ignored */ 
        BfloatQuantEmuFunctor<Device, T>()(
          context->eigen_device<Device>(),
          static_cast<int>(input_tensor.NumElements()),
          input_tensor.flat<T>().data(),
          poutput_tensor->flat<T>().data());
      }
      break; 

    }
  }
 private: 
  string data_format_;
  int allocate_copy_;
  int lpdata_type_;
  int mbits_;
  int exponent_bits_;
  int block_type_;   
  int block_size_;
  int round_mode_;
  int quantize_grad_;
};

template class QuantEmuOp<CPUDevice, float>; 
//template class QuantEmuOp<CPUDevice, Eigen::half>; 
/* Register the CPU kernels. */
#define REGISTER_CPU(T)                                          \
  template struct QuantEmuFunctor<CPUDevice, T>;		 \
  template struct BlockC_QuantEmuFunctor<CPUDevice, T>;		 \
  template struct BlockCHW_QuantEmuFunctor<CPUDevice, T>;	 \
  template struct LowpFloatQuantEmuFunctor<CPUDevice, T>;	 \
  template struct Log2QuantEmuFunctor<CPUDevice, T>;	         \
  template struct PositQuantEmuFunctor<CPUDevice, T>;            \
  template struct BfloatQuantEmuFunctor<CPUDevice, T>;            \
  REGISTER_KERNEL_BUILDER(                                       \
      Name("QuantizeEmu").Device(DEVICE_CPU).TypeConstraint<T>("T"), \
      QuantEmuOp<CPUDevice, T>);

REGISTER_CPU(float);
//REGISTER_CPU(Eigen::half);
#ifdef GOOGLE_CUDA

template class QuantEmuOp<GPUDevice, float>; 
template class QuantEmuOp<GPUDevice, Eigen::half>; 

/* Register the GPU kernels. */ 
#define REGISTER_GPU(T)                                              \
  template struct QuantEmuFunctor<GPUDevice, T>;              	     \
  template struct BlockC_QuantEmuFunctor<GPUDevice, T>;              \
  template struct BlockCHW_QuantEmuFunctor<GPUDevice, T>;            \
  template struct LowpFloatQuantEmuFunctor<GPUDevice, T>;            \
  template struct Log2QuantEmuFunctor<GPUDevice, T>;                 \
  template struct PositQuantEmuFunctor<GPUDevice, T>;                \
  template struct BfloatQuantEmuFunctor<GPUDevice, T>;                \
  REGISTER_KERNEL_BUILDER(                                           \
      Name("QuantizeEmu").Device(DEVICE_GPU).TypeConstraint<T>("T"), \
      QuantEmuOp<GPUDevice, T>);

REGISTER_GPU(float);
REGISTER_GPU(Eigen::half);

#endif /* GOOGLE_CUDA */
