	.ifndef ARCH_PAGING_S__
	.equ ARCH_PAGING_S__, 1
	
	.global paging_invlpg
paging_invlpg:
	invlpg [rdi]
	ret
	
	.endif
