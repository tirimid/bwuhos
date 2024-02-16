	.ifndef ARCH_AUTIL_S__
	.equ ARCH_AUTIL_S__, 1
	
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
	
	.endif
