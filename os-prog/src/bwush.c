#include <stdint.h>

// quick and dirty program to test code loading and scheduler.

static void prog_term(uint64_t rc);

void
_start(void)
{
	prog_term(0);
}

static void
prog_term(uint64_t rc)
{
	__asm__ volatile("\tmov $0x20, %rax\n"
	                 "\tint $0x80\n");
}
