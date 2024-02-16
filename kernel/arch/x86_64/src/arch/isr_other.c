#include "arch/isr_other.h"

void
isr_default_body(void)
{
	// do nothing.
	// maybe in the future it should be recorded that an interrupt occurred,
	// e.g. to help with tracking spurious IRQs?
}
