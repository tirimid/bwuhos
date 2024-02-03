	.ifndef SYS_GDT_S__
	.equ SYS_GDT_S__, 1

	.global gdt_load
gdt_load:
	lgdt (%rdi)

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	pop %rax
	push $0x8
	push %rax

	retfq

	.endif
