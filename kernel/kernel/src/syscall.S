	.ifndef SYSCALL_S
	.equ SYSCALL_S, 1
	
	// this macro and header are expected to exist on all arches.
	// this file and implementation are intentionally kept minimal to reduce
	// porting overhead.
	.include "arch/isr_common.S"
	
	ISR_SYSCALL_DEF_HEAD isr_syscall
	
	.endif
