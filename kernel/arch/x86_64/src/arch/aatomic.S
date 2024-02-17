	.ifndef ARCH_AATOMIC_S
	.equ ARCH_AATOMIC_S, 1
	
	.global aatomic_set
aatomic_set:
	mov sil, 1
.Lcmpxchg_set:
	mov al, 0
	lock cmpxchgb [rdi], sil
	jnz .Lcmpxchg_set
	ret
	
	.global aatomic_clr
aatomic_clr:
	mov sil, 0
.Lcmpxchg_clr:
	mov al, 1
	lock cmpxchgb [rdi], sil
	jnz .Lcmpxchg_clr
	ret
	
	.endif
