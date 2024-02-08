	.ifndef ISR_ISR_COMMON_S__
	.equ ISR_ISR_COMMON_S__, 1
	
	.include "kdef.S"
	
	.macro DEF_ISR_HEAD name
	
	.global \name\()_body
	.global \name\()_head
\name\()_head:
	PUSH_GEN_REGS
	mov rdi, rsp
	call \name\()_body
	POP_GEN_REGS
	iretq
	
	.endm
	
	.macro DEF_EX_HEAD name
	
	.global \name\()_body
	.global \name\()_head
\name\()_head:
	PUSH_GEN_REGS
	mov rdi, rsp
	lea rsi, [rsp + 0x78]
	call \name\()_body
	POP_GEN_REGS
	iretq
	
	.endm
	
	.endif
