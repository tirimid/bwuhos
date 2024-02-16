	.ifndef ARCH_GDT_S
	.equ ARCH_GDT_S, 1

	.global gdt_load
gdt_load:
	lgdt [rdi]

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	pop rax
	push 0x8 // kernel code selector.
	push rax

	retfq

	.endif
