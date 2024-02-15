	.ifndef IDT_S__
	.equ IDT_S__, 1

	.global idt_load
idt_load:
	cli
	lidt [rdi]
	sti
	ret

	.endif
