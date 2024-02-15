	.ifndef ISR_EXCEPT_S__
	.set ISR_EXCEPT_S__, 1
	
	.include "arch/isr_common.S"
	
	ISR_EXCEPT_DEF_HEAD isr_except_0x0
	ISR_EXCEPT_DEF_HEAD isr_except_0x1
	ISR_EXCEPT_DEF_HEAD isr_except_0x2
	ISR_EXCEPT_DEF_HEAD isr_except_0x3
	ISR_EXCEPT_DEF_HEAD isr_except_0x4
	ISR_EXCEPT_DEF_HEAD isr_except_0x5
	ISR_EXCEPT_DEF_HEAD isr_except_0x6
	ISR_EXCEPT_DEF_HEAD isr_except_0x7
	ISR_EXCEPT_DEF_HEAD isr_except_0x8
	ISR_EXCEPT_DEF_HEAD isr_except_0x9
	ISR_EXCEPT_DEF_HEAD isr_except_0xa
	ISR_EXCEPT_DEF_HEAD isr_except_0xb
	ISR_EXCEPT_DEF_HEAD isr_except_0xc
	ISR_EXCEPT_DEF_HEAD isr_except_0xd
	ISR_EXCEPT_DEF_HEAD isr_except_0xe
	ISR_EXCEPT_DEF_HEAD isr_except_0x10
	ISR_EXCEPT_DEF_HEAD isr_except_0x11
	ISR_EXCEPT_DEF_HEAD isr_except_0x12
	ISR_EXCEPT_DEF_HEAD isr_except_0x13
	ISR_EXCEPT_DEF_HEAD isr_except_0x14
	ISR_EXCEPT_DEF_HEAD isr_except_0x15
	ISR_EXCEPT_DEF_HEAD isr_except_0x1c
	ISR_EXCEPT_DEF_HEAD isr_except_0x1d
	ISR_EXCEPT_DEF_HEAD isr_except_0x1e
	
	.endif
