	.ifndef INT_ISR_COMMON_S__
	.equ INT_ISR_COMMON_S__, 1
	
	.include "kdef.S"
	
	.macro DEF_ISR_HEAD name
	
	// the body for a certain name is assumed to exist.
	// if not, then why would you even define a head for it?
	.global \name\()_body
	
	.global \name\()_head
\name\()_head:
	PUSH_GEN_REGS
	call \name\()_body
	POP_GEN_REGS
	iretq
	
	.endm
	
	.endif
