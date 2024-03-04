#include "syscall.h"

#include "kutil.h"

#if defined(K_ARCH_X86_64)
#include "arch/idt.h"
#endif

// set OR* by writing the OR* `args` fields passed into the syscall.
// the ISR definition macro must use this to communicate with the caller.

struct sc_args {
	// refer to `docs/syscalls.md` for argument register meanings.
	uint64_t arsel;
	uint64_t ar0, ar1, ar2, ar3, ar4, ar5, ar6, ar7;
	uint64_t or0, or1, or2, or3;
} __attribute__((packed));

void isr_syscall_head(void);
void isr_syscall_body(struct sc_args *args);

static void sc_dbg_println(struct sc_args *args);
static void sc_prog_term(struct sc_args *args);

void
syscall_init(void)
{
	ku_println(LT_INFO, "syscall: installing ISR");
	
	// 0x80 is syscall interrupt vector.
#if defined(K_ARCH_X86_64)
	idt_set_isr(0x80, (uintptr_t)isr_syscall_head, IGT_INT);
#endif
}

void
isr_syscall_body(struct sc_args *args)
{
	// refer to `docs/syscalls.md` for selection numbers.
	switch (args->arsel) {
	case 0x0:
		sc_dbg_println(args);
		break;
	case 0x20:
		sc_prog_term(args);
		break;
	}
}

static void
sc_dbg_println(struct sc_args *args)
{
	ku_println(args->ar0, "%s", args->ar1);
}

static void
sc_prog_term(struct sc_args *args)
{
	// TODO: implement when scheduler exists.
}
