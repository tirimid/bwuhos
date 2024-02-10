#ifndef ISR_DEV_ISR_H__
#define ISR_DEV_ISR_H__

#include "sys/cpu.h"

// interrupt handlers for hardware like AHCI HBAs.
void di_ahci_hba_head(void);
void di_ahci_hba_body(struct cpu_gen_regs *gr);

#endif
