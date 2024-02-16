	.ifndef ISR_OTHER_S__
	.set ISR_OTHER_S__, 1
	
	.include "arch/isr_common.S"
	
	ISR_DEF_HEAD isr_default
	
	.endif
