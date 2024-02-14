#include "isr/dev_isr.h"

#include "kutil.h"

void
di_ahci_hba_body(struct cpu_gen_regs *gr)
{
	ku_log(LT_DEBUG, "disk xfer interrupt");
}
