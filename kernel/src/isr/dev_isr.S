	.ifndef ISR_DEV_ISR_S__
	.equ ISR_DEV_ISR_S__, 1
	
	.include "isr/isr_common.S"
	
	DEF_ISR_HEAD di_ahci_hba
	
	.endif
