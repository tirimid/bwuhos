#ifndef SYS_CPU_H__
#define SYS_CPU_H__

#include <stdint.h>

struct cpu_gen_regs {
	uint64_t rax, rbx, rcx, rdx;
	uint64_t rsi, rdi;
	uint64_t rbp;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
} __attribute__((packed));

#endif
