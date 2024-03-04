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
	
	.macro ISR_SYSCALL_DEF_HEAD name
	
	.global \name\()_body
	.global \name\()_head
\name\()_head:
	push rdx // OR3.
	push rsi // OR2.
	push rdi // OR1.
	push rax // OR0.
	push r11 // AR7.
	push r10 // AR6.
	push r9 // AR5.
	push r8 // AR4.
	push rcx // AR3.
	push rdx // AR2.
	push rsi // AR1.
	push rdi // AR0.
	push rax // ARSEL.
	
	mov rdi, rsp
	call \name\()_body
	
	// popping the registers like this also has the effect of correctly
	// setting OR*.
	pop rax // ARSEL.
	pop rdi // AR0.
	pop rsi // AR1.
	pop rdx // AR2.
	pop rcx // AR3.
	pop r8 // AR4.
	pop r9 // AR5.
	pop r10 // AR6.
	pop r11 // AR7.
	pop rax // OR0.
	pop rdi // OR1.
	pop rsi // OR2.
	pop rdx // OR3.
	
	iretq
	
	.endm
	
	.endif
