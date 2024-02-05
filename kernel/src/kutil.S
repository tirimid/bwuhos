	.ifndef KUTIL_S__
	.equ KUTIL_S__, 1
	
	.global ku_hang
ku_hang:
	cli
	hlt
	jmp ku_hang
	
	.endif
