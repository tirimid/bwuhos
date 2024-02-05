	.ifndef SYS_PORT_S__
	.equ SYS_PORT_S__, 1

	.global port_rd_8
port_rd_8:
	mov rdx, rdi
	xor rax, rax
	in al, dx
	ret

	.global port_rd_16
port_rd_16:
	mov rdx, rdi
	xor rax, rax
	in ax, dx
	ret

	.global port_rd_32
port_rd_32:
	mov rdx, rdi
	xor rax, rax
	in eax, dx
	ret

	.global port_wr_8
port_wr_8:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret

	.global port_wr_16
port_wr_16:
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	ret

	.global port_wr_32
port_wr_32:
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	ret

	.endif
