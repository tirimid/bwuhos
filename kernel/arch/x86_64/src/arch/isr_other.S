	.ifndef ARCH_ISR_OTHER_S
	.set ARCH_ISR_OTHER_S, 1
	
	.include "arch/isr_common.S"
	
	ISR_DEF_HEAD isr_default
	
	.endif
