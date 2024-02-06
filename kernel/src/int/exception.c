#include "int/exception.h"

#include <stddef.h>
#include <stdint.h>

#include "sys/idt.h"
#include "sys/cpu.h"

#define NULL_ENT {0x0, 0x0}

struct ex_frame {
	uint64_t rip, cs, flags, rsp, ss;
} __attribute__((packed));

// exception handler heads defined in `int/exc.s`.
void exception_0x0_head(void);
void exception_0x1_head(void);
void exception_0x2_head(void);
void exception_0x3_head(void);
void exception_0x4_head(void);
void exception_0x5_head(void);
void exception_0x6_head(void);
void exception_0x7_head(void);
void exception_0x8_head(void);
void exception_0x9_head(void);
void exception_0xa_head(void);
void exception_0xb_head(void);
void exception_0xc_head(void);
void exception_0xd_head(void);
void exception_0xe_head(void);
void exception_0x10_head(void);
void exception_0x11_head(void);
void exception_0x12_head(void);
void exception_0x13_head(void);
void exception_0x14_head(void);
void exception_0x15_head(void);
void exception_0x1c_head(void);
void exception_0x1d_head(void);
void exception_0x1e_head(void);

struct exception_spec const exception_spec_tab[32] = {
	{(uintptr_t)exception_0x0_head, IGT_INT},
	{(uintptr_t)exception_0x1_head, IGT_TRAP},
	{(uintptr_t)exception_0x2_head, IGT_INT},
	{(uintptr_t)exception_0x3_head, IGT_TRAP},
	{(uintptr_t)exception_0x4_head, IGT_TRAP},
	{(uintptr_t)exception_0x5_head, IGT_INT},
	{(uintptr_t)exception_0x6_head, IGT_INT},
	{(uintptr_t)exception_0x7_head, IGT_INT},
	{(uintptr_t)exception_0x8_head, IGT_INT},
	{(uintptr_t)exception_0x9_head, IGT_INT},
	{(uintptr_t)exception_0xa_head, IGT_INT},
	{(uintptr_t)exception_0xb_head, IGT_INT},
	{(uintptr_t)exception_0xc_head, IGT_INT},
	{(uintptr_t)exception_0xd_head, IGT_INT},
	{(uintptr_t)exception_0xe_head, IGT_INT},
	NULL_ENT,
	{(uintptr_t)exception_0x10_head, IGT_INT},
	{(uintptr_t)exception_0x11_head, IGT_INT},
	{(uintptr_t)exception_0x12_head, IGT_INT},
	{(uintptr_t)exception_0x13_head, IGT_INT},
	{(uintptr_t)exception_0x14_head, IGT_INT},
	{(uintptr_t)exception_0x15_head, IGT_INT},
	NULL_ENT,
	NULL_ENT,
	NULL_ENT,
	NULL_ENT,
	NULL_ENT,
	NULL_ENT,
	{(uintptr_t)exception_0x1c_head, IGT_INT},
	{(uintptr_t)exception_0x1d_head, IGT_INT},
	{(uintptr_t)exception_0x1e_head, IGT_INT},
	NULL_ENT,
};

void
exception_0x0_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x1_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x2_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x3_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x4_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x5_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x6_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x7_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x8_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0x9_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0xa_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0xb_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0xc_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0xd_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0xe_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0x10_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x11_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0x12_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x13_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x14_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x15_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0x1c_body(struct cpu_gen_regs gr, struct ex_frame ef)
{
}

void
exception_0x1d_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}

void
exception_0x1e_body(struct cpu_gen_regs gr, uint64_t e, struct ex_frame ef)
{
}
