#include "syscall.h"

#include "fb.h"
#include "kutil.h"

#if defined(K_ARCH_X86_64)
#include "arch/idt.h"
#endif

// set OR* by writing the OR* `args` fields passed into the syscall.
// the ISR definition macro must use this to communicate with the caller.

void
isr_syscall_init(void)
{
	ku_println(LT_INFO, "syscall: installing ISR");
	
	// 0x80 is syscall interrupt vector.
#if defined(K_ARCH_X86_64)
	idt_set_isr(0x80, (uintptr_t)isr_syscall_head, IGT_INT);
#endif
}

void
isr_syscall_body(struct syscall_args *args)
{
	// refer to `docs/syscalls.md` for selection numbers.
	switch (args->arsel) {
	case 0x0:
		syscall_dbg_println(args);
		break;
	case 0x20:
		syscall_prog_term(args);
		break;
	case 0x100:
		syscall_gfx_fb_req_best(args);
		break;
	case 0x140:
		syscall_gfx_fb_put_pixel(args);
		break;
	}
}

void
syscall_dbg_println(struct syscall_args *args)
{
	ku_println(args->ar0, "%s", args->ar1);
}

void
syscall_prog_term(struct syscall_args *args)
{
	// TODO: implement when scheduler exists.
	
	ku_hang();
}

void
syscall_gfx_fb_req_best(struct syscall_args *args)
{
	args->or0 = fb_get_id(fb_get_best());
}

void
syscall_gfx_fb_put_pixel(struct syscall_args *args)
{
	args->or0 = fb_put_pixel(args->ar0, args->ar1, args->ar2, args->ar3,
	                         args->ar4, args->ar5);
}
