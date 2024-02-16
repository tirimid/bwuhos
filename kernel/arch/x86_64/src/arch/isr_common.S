	.ifndef ARCH_ISR_COMMON_S
	.equ ARCH_ISR_COMMON_S, 1
	
	.include "arch/adef.S"
	
	.macro ISR_DEF_HEAD name
	
	.global \name\()_body
	.global \name\()_head
\name\()_head:
	PUSH_GEN_REGS
	mov rdi, rsp
	call \name\()_body
	POP_GEN_REGS
	iretq
	
	.endm
	
	.macro ISR_EXCEPT_DEF_HEAD name
	
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
