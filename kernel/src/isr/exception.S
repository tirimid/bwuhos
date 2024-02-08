	.ifndef ISR_EXCEPTION_S__
	.set ISR_EXCEPTION_S__, 1
	
	.include "isr/isr_common.S"
	
	DEF_EX_HEAD exception_0x0
	DEF_EX_HEAD exception_0x1
	DEF_EX_HEAD exception_0x2
	DEF_EX_HEAD exception_0x3
	DEF_EX_HEAD exception_0x4
	DEF_EX_HEAD exception_0x5
	DEF_EX_HEAD exception_0x6
	DEF_EX_HEAD exception_0x7
	DEF_EX_HEAD exception_0x8
	DEF_EX_HEAD exception_0x9
	DEF_EX_HEAD exception_0xa
	DEF_EX_HEAD exception_0xb
	DEF_EX_HEAD exception_0xc
	DEF_EX_HEAD exception_0xd
	DEF_EX_HEAD exception_0xe
	DEF_EX_HEAD exception_0x10
	DEF_EX_HEAD exception_0x11
	DEF_EX_HEAD exception_0x12
	DEF_EX_HEAD exception_0x13
	DEF_EX_HEAD exception_0x14
	DEF_EX_HEAD exception_0x15
	DEF_EX_HEAD exception_0x1c
	DEF_EX_HEAD exception_0x1d
	DEF_EX_HEAD exception_0x1e
	
	.endif
