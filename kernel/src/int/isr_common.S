	.ifndef INT_ISR_COMMON_S__
	.equ INT_ISR_COMMON_S__, 1
	
	.macro DEF_ISR_HEAD name
	
	// the body for a certain name is assumed to exist.
	// if not, then why would you even define a head for it?
	.global \name\()_body
	
	.global \name\()_head
\name\()_head:
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rbp
	push rdi
	push rsi
	push rdx
	push rcx
	push rbx
	push rax
	
	call \name\()_body
	
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	
	iretq
	
	.endm
	
	.endif
