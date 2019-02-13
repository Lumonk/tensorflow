# Convolution 
ENABLE_QUANTOP_CONV=1
ENABLE_QUANTOP_CONV_GRAD=1

# BatchNormalization
ENABLE_QUANTOP_BNORM=1
ENABLE_QUANTOP_BNORM_GRAD=1

# MATMUL 
ENABLE_QUANTOP_MATMUL=0
ENABLE_QUANTOP_MATMUL_GRAD=0

# MUL OP  
ENABLE_QUANTOP_MUL=0
ENABLE_QUANTOP_MUL_GRAD=0

# LSTM 
ENABLE_QUANTOP_LSTM=0
ENABLE_QUANTOP_LSTM_GATES=0
ENABLE_QUANTGRAD_LSTM_GRAD=0

# Data Type Settings 
# INT=1, UINT=2, LOWP_FP=3, LOG2=4, POSIT=5 
QUANTEMU_INPUT_DATA_TYPE=3
QUANTEMU_FILTER_DATA_TYPE=3
QUANTEMU_GRAD_DATA_TYPE=3

# only used by LOWP_FP, POSIT and BLOCK_FP types 
QUANTEMU_EXPBITS=5          

# Rounding modes  
# Truncate (no rounding)=0, Round to Nearest Even(RNE)=1, STOCHASTIC_ROUNDING=2  
QUANTEMU_RMODE_INPUTS=2
QUANTEMU_RMODE_FILTERS=2
QUANTEMU_RMODE_GRADS=2

# Precision Settings 
QUANTEMU_FIRST_LAYER_PRECISION=16

QUANTEMU_PRECISION_CONV_INPUTS=8
QUANTEMU_PRECISION_CONV_FILTERS=8
QUANTEMU_PRECISION_CONV_OUTPUTS=8
QUANTEMU_PRECISION_CONV_GRADS=8
QUANTEMU_PRECISION_BNORM_INPUTS=8
QUANTEMU_PRECISION_BNORM_GRADS=8

QUANTEMU_PRECISION_LSTM_INPUTS=8
QUANTEMU_PRECISION_LSTM_FILTERS=8
QUANTEMU_PRECISION_LSTM_GRADS=8

QUANTEMU_PRECISION_MATMUL_INPUTS=8
QUANTEMU_PRECISION_MATMUL_FILTERS=8
QUANTEMU_PRECISION_MATMUL_OUTPUTS=8
QUANTEMU_PRECISION_MATMUL_GRADS=8

# Buffer Copy Settings 
# Make a Copy while Quantizing 
QUANTEMU_ALLOCATE_COPY_INPUTS=0
QUANTEMU_ALLOCATE_COPY_FILTERS=1
QUANTEMU_ALLOCATE_COPY_OUTPUTS=0
QUANTEMU_ALLOCATE_COPY_GRADS=0

# FGQ Settings 
# NOBLOCK=0, BLOCK_C=1, BLOCK_CHW=2 
QUANTEMU_CBLOCK_TYPE_CONV_INPUTS=0
QUANTEMU_CBLOCK_TYPE_CONV_FILTERS=0
QUANTEMU_CBLOCK_TYPE_CONV_OUTPUTS=0
QUANTEMU_CBLOCK_TYPE_CONV_GRADS=0

QUANTEMU_CBLOCK_TYPE_BNORM_INPUTS=0
QUANTEMU_CBLOCK_TYPE_BNORM_GRADS=0

QUANTEMU_CBLOCK_SIZE_INPUTS=2048
QUANTEMU_CBLOCK_SIZE_FILTER=2048
QUANTEMU_CBLOCK_SIZE_OUTPUTS=2048
QUANTEMU_CBLOCK_SIZE_GRAD=2048

export ENABLE_QUANTOP_CONV
export ENABLE_QUANTOP_CONV_GRAD
export ENABLE_QUANTOP_BNORM
export ENABLE_QUANTOP_BNORM_GRAD
export ENABLE_QUANTOP_MATMUL
export ENABLE_QUANTOP_MATMUL_GRAD
export ENABLE_QUANTOP_MUL
export ENABLE_QUANTOP_MUL_GRAD
export ENABLE_QUANTOP_LSTM
export ENABLE_QUANTOP_LSTM_GATES
export ENABLE_QUANTGRAD_LSTM_GRAD
export QUANTEMU_INPUT_DATA_TYPE
export QUANTEMU_FILTER_DATA_TYPE
export QUANTEMU_GRAD_DATA_TYPE
export QUANTEMU_EXPBITS          
export QUANTEMU_RMODE_INPUTS
export QUANTEMU_RMODE_FILTERS
export QUANTEMU_RMODE_GRADS
export QUANTEMU_FIRST_LAYER_PRECISION
export QUANTEMU_PRECISION_CONV_INPUTS
export QUANTEMU_PRECISION_CONV_FILTERS
export QUANTEMU_PRECISION_CONV_OUTPUTS
export QUANTEMU_PRECISION_CONV_GRADS
export QUANTEMU_PRECISION_BNORM_INPUTS
export QUANTEMU_PRECISION_BNORM_GRADS
export QUANTEMU_PRECISION_LSTM_INPUTS
export QUANTEMU_PRECISION_LSTM_FILTERS
export QUANTEMU_PRECISION_LSTM_GRADS
export QUANTEMU_PRECISION_MATMUL_INPUTS
export QUANTEMU_PRECISION_MATMUL_FILTERS
export QUANTEMU_PRECISION_MATMUL_OUTPUTS
export QUANTEMU_PRECISION_MATMUL_GRADS
export QUANTEMU_ALLOCATE_COPY_INPUTS
export QUANTEMU_ALLOCATE_COPY_FILTERS
export QUANTEMU_ALLOCATE_COPY_OUTPUTS
export QUANTEMU_ALLOCATE_COPY_GRADS
export QUANTEMU_CBLOCK_TYPE_CONV_INPUTS
export QUANTEMU_CBLOCK_TYPE_CONV_FILTERS
export QUANTEMU_CBLOCK_TYPE_CONV_OUTPUTS
export QUANTEMU_CBLOCK_TYPE_CONV_GRADS
export QUANTEMU_CBLOCK_TYPE_BNORM_INPUTS
export QUANTEMU_CBLOCK_TYPE_BNORM_GRADS
export QUANTEMU_CBLOCK_SIZE_INPUTS
export QUANTEMU_CBLOCK_SIZE_FILTER
export QUANTEMU_CBLOCK_SIZE_OUTPUTS
export QUANTEMU_CBLOCK_SIZE_GRAD
