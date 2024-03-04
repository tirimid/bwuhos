	.ifndef ARCH_CPU_S
	.equ ARCH_CPU_S, 1
	
	.global cpu_get_ctl_regs
cpu_get_ctl_regs:
	mov rax, cr0
	mov [rdi], rax
	mov rax, cr2
	mov [rdi + 0x8], rax
	mov rax, cr3
	mov [rdi + 0x10], rax
	mov rax, cr4
	mov [rdi + 0x18], rax
	mov rax, cr8
	mov [rdi + 0x20], rax
	
	mov ecx, 0xc0000080
	rdmsr
	shl rdx, 32
	or rax, rdx
	mov [rdi + 0x28], rax
	
	mov rax, rdi
	
	ret
	
	.global cpu_switch_mem_ctx
cpu_switch_mem_ctx:
	mov rbp, rsi
	mov rsp, rsi
	mov cr3, rdi
	jmp rdx
	
	.endif
