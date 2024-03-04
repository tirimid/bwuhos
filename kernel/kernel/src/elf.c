#include "elf.h"

#include "kdef.h"
#include "kheap.h"
#include "kutil.h"
#include "pmm.h"

// using an integer here allows a single efficient 32-bit comparison to be
// performed on MAG0..=3 rather than checking each one in order.
#define ELF_MAGIC 0x464c457f

// only ELF 64 is implemented here - ELF 32 is unsupported.

typedef uint64_t elf_addr_t;
typedef uint64_t elf_off_t;
typedef uint16_t elf_half_t;
typedef uint32_t elf_word_t;
typedef int32_t elf_sword_t;
typedef uint64_t elf_xword_t;
typedef int64_t elf_sxword_t;

enum phdr_type {
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
};

enum phdr_flag {
	PF_X = 0x1,
	PF_W = 0x2,
	PF_R = 0x4,
};

struct elf_hdr {
	char e_ident[16];
	elf_half_t e_type;
	elf_half_t e_machine;
	elf_word_t e_version;
	elf_addr_t e_entry;
	elf_off_t e_phoff;
	elf_off_t e_shoff;
	elf_word_t e_flags;
	elf_half_t e_ehsize;
	elf_half_t e_phentsize;
	elf_half_t e_phnum;
	elf_half_t e_shentsize;
	elf_half_t e_shnum;
	elf_half_t e_shstrndx;
} __attribute__((packed));

struct elf_phdr {
	elf_word_t p_type;
	elf_word_t p_flags;
	elf_off_t p_offset;
	elf_addr_t p_vaddr;
	elf_addr_t p_paddr;
	elf_xword_t p_filesz;
	elf_xword_t p_memsz;
	elf_xword_t p_align;
} __attribute__((packed));

int
elf_load(uintptr_t *out_entry, page_map_t map, struct vfs_file *file)
{
	// TODO: implement.
}
