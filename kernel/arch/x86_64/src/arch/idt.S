	.ifndef ARCH_IDT_S
	.equ ARCH_IDT_S, 1

	.global idt_load
idt_load:
	cli
	lidt [rdi]
	sti
	ret

	.endif
