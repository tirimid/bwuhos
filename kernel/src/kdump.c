#include "kdump.h"

#include "kutil.h"

void
kdump_gen_regs(struct cpu_gen_regs const *gr)
{
	ku_log(LT_ERR, "kernel general register dump");
	ku_log(LT_ERR, "|  rax  0x%x = 0b%b", gr->rax, gr->rax);
	ku_log(LT_ERR, "|  rbx  0x%x = 0b%b", gr->rbx, gr->rbx);
	ku_log(LT_ERR, "|  rcx  0x%x = 0b%b", gr->rcx, gr->rcx);
	ku_log(LT_ERR, "|  rdx  0x%x = 0b%b", gr->rdx, gr->rdx);
	ku_log(LT_ERR, "|  rsi  0x%x = 0b%b", gr->rsi, gr->rsi);
	ku_log(LT_ERR, "|  rdi  0x%x = 0b%b", gr->rdi, gr->rdi);
	ku_log(LT_ERR, "|  rbp  0x%x = 0b%b", gr->rbp, gr->rbp);
	ku_log(LT_ERR, "|  r8   0x%x = 0b%b", gr->r8, gr->r8);
	ku_log(LT_ERR, "|  r9   0x%x = 0b%b", gr->r9, gr->r9);
	ku_log(LT_ERR, "|  r10  0x%x = 0b%b", gr->r10, gr->r10);
	ku_log(LT_ERR, "|  r11  0x%x = 0b%b", gr->r11, gr->r11);
	ku_log(LT_ERR, "|  r12  0x%x = 0b%b", gr->r12, gr->r12);
	ku_log(LT_ERR, "|  r13  0x%x = 0b%b", gr->r13, gr->r13);
	ku_log(LT_ERR, "|  r14  0x%x = 0b%b", gr->r14, gr->r14);
	ku_log(LT_ERR, ":  r15  0x%x = 0b%b", gr->r15, gr->r15);
}

void
kdump_ctl_regs(struct cpu_ctl_regs const *cr)
{
	ku_log(LT_ERR, "kernel control register dump");
	ku_log(LT_ERR, "|  cr0   0x%x = 0b%b", cr->cr0, cr->cr0);
	ku_log(LT_ERR, "|  cr2   0x%x = 0b%b", cr->cr2, cr->cr2);
	ku_log(LT_ERR, "|  cr3   0x%x = 0b%b", cr->cr3, cr->cr3);
	ku_log(LT_ERR, "|  cr4   0x%x = 0b%b", cr->cr4, cr->cr4);
	ku_log(LT_ERR, "|  cr8   0x%x = 0b%b", cr->cr8, cr->cr8);
	ku_log(LT_ERR, ":  efer  0x%x = 0b%b", cr->efer, cr->efer);
}

void
kdump_exception_no_code(struct ex_frame_no_code const *ef)
{
	ku_log(LT_ERR, "kernel exception dump (no code)");
	ku_log(LT_ERR, "|  rip    0x%x = 0b%b", ef->rip, ef->rip);
	ku_log(LT_ERR, "|  cs     0x%x = 0b%b", ef->cs, ef->cs);
	ku_log(LT_ERR, "|  flags  0x%x = 0b%b", ef->flags, ef->flags);
	ku_log(LT_ERR, "|  rsp    0x%x = 0b%b", ef->rsp, ef->rsp);
	ku_log(LT_ERR, ":  ss     0x%x = 0b%b", ef->ss, ef->ss);
}

void
kdump_exception_code(struct ex_frame_code const *ef)
{
	ku_log(LT_ERR, "kernel exception dump (code)");
	ku_log(LT_ERR, "|  e      0x%x = 0b%b", ef->e, ef->e);
	ku_log(LT_ERR, "|  rip    0x%x = 0b%b", ef->rip, ef->rip);
	ku_log(LT_ERR, "|  cs     0x%x = 0b%b", ef->cs, ef->cs);
	ku_log(LT_ERR, "|  flags  0x%x = 0b%b", ef->flags, ef->flags);
	ku_log(LT_ERR, "|  rsp    0x%x = 0b%b", ef->rsp, ef->rsp);
	ku_log(LT_ERR, ":  ss     0x%x = 0b%b", ef->ss, ef->ss);
}
