	.ifndef INT_EXCEPTION_S__
	.set INT_EXCEPTION_S__, 1
	
	.include "int/isr_common.S"
	
	DEF_ISR_HEAD exception_0x0
	DEF_ISR_HEAD exception_0x1
	DEF_ISR_HEAD exception_0x2
	DEF_ISR_HEAD exception_0x3
	DEF_ISR_HEAD exception_0x4
	DEF_ISR_HEAD exception_0x5
	DEF_ISR_HEAD exception_0x6
	DEF_ISR_HEAD exception_0x7
	DEF_ISR_HEAD exception_0x8
	DEF_ISR_HEAD exception_0x9
	DEF_ISR_HEAD exception_0xa
	DEF_ISR_HEAD exception_0xb
	DEF_ISR_HEAD exception_0xc
	DEF_ISR_HEAD exception_0xd
	DEF_ISR_HEAD exception_0xe
	DEF_ISR_HEAD exception_0x10
	DEF_ISR_HEAD exception_0x11
	DEF_ISR_HEAD exception_0x12
	DEF_ISR_HEAD exception_0x13
	DEF_ISR_HEAD exception_0x14
	DEF_ISR_HEAD exception_0x15
	DEF_ISR_HEAD exception_0x1c
	DEF_ISR_HEAD exception_0x1d
	DEF_ISR_HEAD exception_0x1e
	
	.endif
