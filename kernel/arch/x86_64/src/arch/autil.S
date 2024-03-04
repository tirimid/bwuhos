	.ifndef ARCH_AUTIL_S
	.equ ARCH_AUTIL_S, 1
	
	.global au_hang
au_hang:
	cli
	hlt
	jmp au_hang
	
	.global au_fms_64
au_fms_64:
	mov rcx, rdx
	mov rax, rsi
	rep stosq
	ret
	
	.global au_fmc_64
au_fmc_64:
	mov rcx, rdx
	rep movsq
	ret
	
	.endif
