	.ifndef ARCH_PAGING_S
	.equ ARCH_PAGING_S, 1
	
	.global paging_invlpg
paging_invlpg:
	invlpg [rdi]
	ret
	
	.endif
