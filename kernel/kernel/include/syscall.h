#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

struct syscall_args {
	uint64_t arsel;
	uint64_t ar0, ar1, ar2, ar3, ar4, ar5, ar6, ar7;
	uint64_t or0, or1, or2, or3;
};

// NOT an ISR.
// call this to install the syscall ISR to the interrupt vector.
void isr_syscall_init(void);

void isr_syscall_head(void);
void isr_syscall_body(struct syscall_args *args);

void syscall_dbg_println(struct syscall_args *args);
void syscall_prog_term(struct syscall_args *args);
void syscall_gfx_fb_req_best(struct syscall_args *args);
void syscall_gfx_fb_put_pixel(struct syscall_args *args);

#endif
