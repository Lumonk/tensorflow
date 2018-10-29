# Convolution 
ENABLE_QUANTOP_CONV_INPUTS=1
ENABLE_QUANTOP_CONV_OUTPUTS=1
ENABLE_QUANTGRAD_CONV_INPUTS=0
ENABLE_QUANTGRAD_CONV_OUTPUTS=0
QUANTEMU_SKIP_FIRST_LAYER=0 

# LSTM 
ENABLE_QUANTOP_LSTM=0
ENABLE_QUANTOP_MATMUL_INPUTS=0
ENABLE_QUANTOP_LSTM_GATES=0
ENABLE_QUANTGRAD_LSTM=0
ENABLE_QUANTGRAD_LSTM_GATES=0
ENABLE_QUANTOP_LSTM_BFLOAT=0
ENABLE_QUANTOP_LSTM_GATES_BFLOAT=0

# Desnse Layer 
ENABLE_QUANTOP_DENSE_INPUTS=0
ENABLE_QUANTOP_DENSE_OUTPUTS=0
ENABLE_QUANTOPGRAD_DENSE_INPUTS=0
ENABLE_QUANTOPGRAD_DENSE_OUTPUTS=0

# Multiply Layer 
ENABLE_QUANTOP_MULTIPLY_INPUTS=0
ENABLE_QUANTOPGRAD_MULTIPLY_INPUTS=0
ENABLE_QUANTOP_MULTIPLY_OUTPUTS=0
ENABLE_QUANTOPGRAD_MULTIPLY_OUTPUTS=0


# DFP_INT=1, LOWP_FP=2, POSIT=3 
QUANTEMU_LPDATA_TYPE=1
# Unsigned activations (weights remain signed )
QUANTEMU_CONV_USIGNED_ACTS=0 

QUANTEMU_FIRST_LAYER_PRECISION=8
QUANTEMU_PRECISION_INPUTS=8
QUANTEMU_PRECISION_FILTER=4
QUANTEMU_PRECISION_OUTPUTS=8
QUANTEMU_PRECISION_GRAD=23
QUANTEMU_PRECISION_MATMUL_INPUTS=8
QUANTEMU_PRECISION_MATMUL_OUTPUTS=8
QUANTEMU_PRECISION_DENSE_INPUTS=8
QUANTEMU_PRECISION_DENSE_FILTER=8
QUANTEMU_PRECISION_DENSE_OUTPUTS=8
QUANTEMU_EXPBITS=5          # only used by LOWP_FP, BLOCK_FP types 

# Make a copy while quantizing 
QUANTEMU_ALLOCATE_COPY_INPUTS=0
QUANTEMU_ALLOCATE_COPY_FILTER=1
QUANTEMU_ALLOCATE_COPY_OUTPUTS=0
QUANTEMU_ALLOCATE_COPY_GRAD=0

# 0=None, 1=BIASED, 2=NEAREST 
QUANTEMU_RMODE_INPUTS=0
QUANTEMU_RMODE_FILTER=0
QUANTEMU_RMODE_OUTPUTS=0
QUANTEMU_RMODE_GRAD=0

# NOBLOCK=0, BLOCK_C=1, BLOCK_CHW=2 
QUANTEMU_CBLOCK_TYPE_CONV_INPUTS=2
QUANTEMU_CBLOCK_TYPE_CONV_FILTER=2
QUANTEMU_CBLOCK_TYPE_CONV_OUTPUTS=2
QUANTEMU_CBLOCK_TYPE_CONV_GRAD=0

QUANTEMU_CBLOCK_TYPE_DENSE_INPUTS=0
QUANTEMU_CBLOCK_TYPE_DENSE_FILTER=0
QUANTEMU_CBLOCK_TYPE_DENSE_OUTPUTS=0
QUANTEMU_CBLOCK_TYPE_DENSE_GRAD=0

QUANTEMU_CBLOCK_SIZE_INPUTS=2048
QUANTEMU_CBLOCK_SIZE_FILTER=2048
QUANTEMU_CBLOCK_SIZE_OUTPUTS=2048
QUANTEMU_CBLOCK_SIZE_GRAD=2048

export ENABLE_QUANTOP_DENSE_INPUTS
export ENABLE_QUANTOP_DENSE_OUTPUTS
export ENABLE_QUANTOPGRAD_DENSE_INPUTS
export ENABLE_QUANTOPGRAD_DENSE_OUTPUTS

export ENABLE_QUANTOP_MULTIPLY_INPUTS
export ENABLE_QUANTOPGRAD_MULTIPLY_INPUTS
export ENABLE_QUANTOP_MULTIPLY_OUTPUTS
export ENABLE_QUANTOPGRAD_MULTIPLY_OUTPUTS

export QUANTEMU_ALLOCATE_COPY_INPUTS
export QUANTEMU_ALLOCATE_COPY_FILTER
export QUANTEMU_ALLOCATE_COPY_OUTPUTS
export QUANTEMU_ALLOCATE_COPY_GRAD

export ENABLE_QUANTOP_CONV_INPUTS
export ENABLE_QUANTOP_CONV_OUTPUTS
export ENABLE_QUANTGRAD_CONV_INPUTS
export ENABLE_QUANTGRAD_CONV_OUTPUTS
export QUANTEMU_SKIP_FIRST_LAYER 

export ENABLE_QUANTOP_LSTM
export ENABLE_QUANTOP_MATMUL_INPUTS
export ENABLE_QUANTOP_LSTM_GATES
export ENABLE_QUANTGRAD_LSTM
export ENABLE_QUANTGRAD_LSTM_GATES
export ENABLE_QUANTOP_LSTM_BFLOAT
export ENABLE_QUANTOP_LSTM_GATES_BFLOAT

export QUANTEMU_LPDATA_TYPE
export QUANTEMU_CONV_USIGNED_ACTS
export QUANTEMU_FIRST_LAYER_PRECISION
export QUANTEMU_PRECISION_INPUTS
export QUANTEMU_PRECISION_FILTER
export QUANTEMU_PRECISION_OUTPUTS
export QUANTEMU_PRECISION_GRAD
export QUANTEMU_PRECISION_MATMUL_INPUTS
export QUANTEMU_PRECISION_MATMUL_OUTPUTS
export QUANTEMU_PRECISION_DENSE_INPUTS
export QUANTEMU_PRECISION_DENSE_FILTER
export QUANTEMU_PRECISION_DENSE_OUTPUTS
export QUANTEMU_EXPBITS        

export QUANTEMU_CBLOCK_TYPE_CONV_INPUTS
export QUANTEMU_CBLOCK_TYPE_CONV_OUTPUTS
export QUANTEMU_CBLOCK_TYPE_CONV_FILTER
export QUANTEMU_CBLOCK_TYPE_CONV_GRAD

export QUANTEMU_CBLOCK_TYPE_DENSE_INPUTS
export QUANTEMU_CBLOCK_TYPE_DENSE_FILTER
export QUANTEMU_CBLOCK_TYPE_DENSE_OUTPUTS
export QUANTEMU_CBLOCK_TYPE_DENSE_GRAD

export QUANTEMU_CBLOCK_SIZE_INPUTS
export QUANTEMU_CBLOCK_SIZE_OUTPUTS
export QUANTEMU_CBLOCK_SIZE_FILTER
export QUANTEMU_CBLOCK_SIZE_GRAD

export QUANTEMU_RMODE_INPUTS
export QUANTEMU_RMODE_OUTPUTS
export QUANTEMU_RMODE_FILTER
export QUANTEMU_RMODE_GRAD
