#ifndef KDUMP_H__
#define KDUMP_H__

#include <stdint.h>

#include "isr/exception.h"
#include "sys/cpu.h"

void kdump_gen_regs(struct cpu_gen_regs const *gr);
void kdump_ctl_regs(struct cpu_ctl_regs const *cr);
void kdump_exception_no_code(struct ex_frame_no_code const *ef);
void kdump_exception_code(struct ex_frame_code const *ef);
void kdump_ata_id(uint16_t const *id_buf);

#endif
