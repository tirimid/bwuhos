	.ifndef SYS_IDT_S__
	.equ SYS_IDT_S__, 1

	.global idt_load
idt_load:
	cli
	lidt (%rdi)
	sti
	ret

	.endif
