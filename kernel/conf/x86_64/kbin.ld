OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHDRS
{
	text PT_LOAD FLAGS(1 << 0 | 1 << 2); /* exec + read. */
	rodata PT_LOAD FLAGS(1 << 2); /* read. */
	data PT_LOAD FLAGS(1 << 1 | 1 << 2); /* write + read. */
}

SECTIONS
{
	. = 0xffffffff80000000;

	K_START_VADDR = .;

	.text : {
		*(.text .text.*)
	} :text

	. += CONSTANT(MAXPAGESIZE);
	.rodata : {
		*(.rodata .rodata.*)
	} :rodata

	. += CONSTANT(MAXPAGESIZE);
	.data : {
		*(.data .data.*)
	} :data

	.bss : {
		*(COMMON)
		*(.bss .bss.*)
	} :data

	K_END_VADDR = .;

	/DISCARD/ : {
		*(.eh_frame)
		*(.note .note.*)
	}
}
