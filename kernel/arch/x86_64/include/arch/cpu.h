#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include <stdint.h>

#include "kdef.h"

struct cpu_gen_regs {
	uint64_t rax, rbx, rcx, rdx;
	uint64_t rsi, rdi;
	uint64_t rbp;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
} __attribute__((packed));

struct cpu_ctl_regs {
	uint64_t cr0, cr2, cr3, cr4, cr8;
	uint64_t efer;
} __attribute__((packed));

struct cpu_ctl_regs cpu_get_ctl_regs(void);
void cpu_switch_mem_ctx(phys_addr_t cr3, void const *stk_base, void (*post_load_jmp)(void));

#endif
