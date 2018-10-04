# Convolution 
ENABLE_QUANTOP_CONV_INPUTS=1
ENABLE_QUANTOP_CONV_OUTPUTS=0
ENABLE_QUANTGRAD_CONV_INPUTS=0
ENABLE_QUANTGRAD_CONV_OUTPUTS=0
QUANTEMU_SKIP_FIRST_LAYER=0 

# LSTM 
ENABLE_QUANTOP_LSTM=0
ENABLE_QUANTOP_LSTM_GATES=0
ENABLE_QUANTOP_LSTM_BFLOAT=0
ENABLE_QUANTOP_LSTM_GATES_BFLOAT=0
ENABLE_QUANTGRAD_LSTM=0
ENABLE_QUANTGRAD_LSTM_GATES=0

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


# DFP_INT=0, DFP_UINT=1, TERNARY=2, BLOCK_FP=3, LOWP_FP=4
QUANTEMU_LPDATA_TYPE=4

QUANTEMU_PRECISION_INPUTS=8
QUANTEMU_PRECISION_FILTER=8
QUANTEMU_PRECISION_OUTPUTS=8
QUANTEMU_PRECISION_GRAD=23
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
QUANTEMU_CBLOCK_TYPE_INPUTS=0
QUANTEMU_CBLOCK_TYPE_FILTER=0
QUANTEMU_CBLOCK_TYPE_OUTPUTS=0
QUANTEMU_CBLOCK_TYPE_GRAD=0

QUANTEMU_CBLOCK_SIZE_INPUTS=4096
QUANTEMU_CBLOCK_SIZE_FILTER=4096
QUANTEMU_CBLOCK_SIZE_OUTPUTS=4096
QUANTEMU_CBLOCK_SIZE_GRAD=128


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
export ENABLE_QUANTOP_LSTM_GATES
export ENABLE_QUANTGRAD_LSTM
export ENABLE_QUANTGRAD_LSTM_GATES
export ENABLE_QUANTOP_LSTM_BFLOAT
export ENABLE_QUANTOP_LSTM_GATES_BFLOAT

export QUANTEMU_LPDATA_TYPE
export QUANTEMU_PRECISION_INPUTS
export QUANTEMU_PRECISION_FILTER
export QUANTEMU_PRECISION_OUTPUTS
export QUANTEMU_PRECISION_GRAD
export QUANTEMU_EXPBITS        

export QUANTEMU_CBLOCK_TYPE_INPUTS
export QUANTEMU_CBLOCK_TYPE_OUTPUTS
export QUANTEMU_CBLOCK_TYPE_FILTER
export QUANTEMU_CBLOCK_TYPE_GRAD

export QUANTEMU_CBLOCK_SIZE_INPUTS
export QUANTEMU_CBLOCK_SIZE_OUTPUTS
export QUANTEMU_CBLOCK_SIZE_FILTER
export QUANTEMU_CBLOCK_SIZE_GRAD

export QUANTEMU_RMODE_INPUTS
export QUANTEMU_RMODE_OUTPUTS
export QUANTEMU_RMODE_FILTER
export QUANTEMU_RMODE_GRAD
