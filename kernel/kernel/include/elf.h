#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#include "vfs.h"
#include "vmm.h"

// TODO: make ELF work properly (implement linking etc.).

int elf_load(uintptr_t *out_entry, page_map_t map, struct vfs_file *file);

#endif
