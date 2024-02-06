	.ifndef KUTIL_S__
	.equ KUTIL_S__, 1
	
	.global ku_hang
ku_hang:
	cli
	hlt
	jmp ku_hang
	
	.global ku_fms_8
ku_fms_8:
	mov rcx, rdx
	mov rax, rsi
	rep stosb
	ret
	
	.global ku_fms_16
ku_fms_16:
	mov rcx, rdx
	mov rax, rsi
	rep stosw
	ret
	
	.global ku_fms_32
ku_fms_32:
	mov rcx, rdx
	mov rax, rsi
	rep stosd
	ret
	
	.global ku_fms_64
ku_fms_64:
	mov rcx, rdx
	mov rax, rsi
	rep stosq
	ret
	
	.endif
