#include "kdump.h"

#include "kutil.h"

void
kdump_gen_regs(struct cpu_gen_regs const *gr)
{
	ku_log(LT_INFO, "kernel general register dump");
	ku_log(LT_NONE, "|  rax  0x%Qx  0b%Qb", gr->rax, gr->rax);
	ku_log(LT_NONE, "|  rbx  0x%Qx  0b%Qb", gr->rbx, gr->rbx);
	ku_log(LT_NONE, "|  rcx  0x%Qx  0b%Qb", gr->rcx, gr->rcx);
	ku_log(LT_NONE, "|  rdx  0x%Qx  0b%Qb", gr->rdx, gr->rdx);
	ku_log(LT_NONE, "|  rsi  0x%Qx  0b%Qb", gr->rsi, gr->rsi);
	ku_log(LT_NONE, "|  rdi  0x%Qx  0b%Qb", gr->rdi, gr->rdi);
	ku_log(LT_NONE, "|  rbp  0x%Qx  0b%Qb", gr->rbp, gr->rbp);
	ku_log(LT_NONE, "|  r8   0x%Qx  0b%Qb", gr->r8, gr->r8);
	ku_log(LT_NONE, "|  r9   0x%Qx  0b%Qb", gr->r9, gr->r9);
	ku_log(LT_NONE, "|  r10  0x%Qx  0b%Qb", gr->r10, gr->r10);
	ku_log(LT_NONE, "|  r11  0x%Qx  0b%Qb", gr->r11, gr->r11);
	ku_log(LT_NONE, "|  r12  0x%Qx  0b%Qb", gr->r12, gr->r12);
	ku_log(LT_NONE, "|  r13  0x%Qx  0b%Qb", gr->r13, gr->r13);
	ku_log(LT_NONE, "|  r14  0x%Qx  0b%Qb", gr->r14, gr->r14);
	ku_log(LT_NONE, "|  r15  0x%Qx  0b%Qb", gr->r15, gr->r15);
}

void
kdump_ctl_regs(struct cpu_ctl_regs const *cr)
{
	ku_log(LT_INFO, "kernel control register dump");
	ku_log(LT_NONE, "|  cr0   0x%Qx  0b%Qb", cr->cr0, cr->cr0);
	ku_log(LT_NONE, "|  cr2   0x%Qx  0b%Qb", cr->cr2, cr->cr2);
	ku_log(LT_NONE, "|  cr3   0x%Qx  0b%Qb", cr->cr3, cr->cr3);
	ku_log(LT_NONE, "|  cr4   0x%Qx  0b%Qb", cr->cr4, cr->cr4);
	ku_log(LT_NONE, "|  cr8   0x%Qx  0b%Qb", cr->cr8, cr->cr8);
	ku_log(LT_NONE, "|  efer  0x%Qx  0b%Qb", cr->efer, cr->efer);
}

void
kdump_exception_no_code(struct ex_frame_no_code const *ef)
{
	ku_log(LT_INFO, "kernel exception dump (no code)");
	ku_log(LT_NONE, "|  rip    0x%Qx  0b%Qb", ef->rip, ef->rip);
	ku_log(LT_NONE, "|  cs     0x%Qx  0b%Qb", ef->cs, ef->cs);
	ku_log(LT_NONE, "|  flags  0x%Qx  0b%Qb", ef->flags, ef->flags);
	ku_log(LT_NONE, "|  rsp    0x%Qx  0b%Qb", ef->rsp, ef->rsp);
	ku_log(LT_NONE, "|  ss     0x%Qx  0b%Qb", ef->ss, ef->ss);
}

void
kdump_exception_code(struct ex_frame_code const *ef)
{
	ku_log(LT_INFO, "kernel exception dump (code)");
	ku_log(LT_NONE, "|  e      0x%Qx  0b%Qb", ef->e, ef->e);
	ku_log(LT_NONE, "|  rip    0x%Qx  0b%Qb", ef->rip, ef->rip);
	ku_log(LT_NONE, "|  cs     0x%Qx  0b%Qb", ef->cs, ef->cs);
	ku_log(LT_NONE, "|  flags  0x%Qx  0b%Qb", ef->flags, ef->flags);
	ku_log(LT_NONE, "|  rsp    0x%Qx  0b%Qb", ef->rsp, ef->rsp);
	ku_log(LT_NONE, "|  ss     0x%Qx  0b%Qb", ef->ss, ef->ss);
}

void
kdump_ata_id(uint16_t const *id_buf)
{
	ku_log(LT_INFO, "kernel ATA identity dump");
	for (size_t i = 0; i < 256; i += 8) {
		ku_log(LT_NONE, "|  0x%Wx 0x%Wx 0x%Wx 0x%Wx 0x%Wx 0x%Wx 0x%Wx 0x%Wx",
		       id_buf[i], id_buf[i + 1], id_buf[i + 2], id_buf[i + 3],
		       id_buf[i + 4], id_buf[i + 5], id_buf[i + 6],
		       id_buf[i + 7]);
	}
}
