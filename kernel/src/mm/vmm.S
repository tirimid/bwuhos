	.ifndef MM_VMM_S__
	.equ MM_VMM_S__, 1
	
	.global vmm_invlpg
vmm_invlpg:
	invlpg [rdi]
	ret
	
	.endif
