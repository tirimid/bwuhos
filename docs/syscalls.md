# bwuhos system calls

## System call model

Arguments are passed into CPU registers (AR*) and the system call software
interrupt is raised (0x80). Results are returned in CPU registers (OR*).
Syscalls do not preserve registers.

For the purpose of being architecture-agnostic, these registers will not be
referred to directly by their names, but mapped to generic "argument registers",
as follows:

### x86_64

| Argument register	| CPU register	|
|-----------------------|---------------|
| ARSEL			| rax		|
| AR0			| rdi		|
| AR1			| rsi		|
| AR2			| rdx		|
| AR3			| rcx		|
| AR4			| r8		|
| AR5			| r9		|
| AR6			| r10		|
| AR7			| r11		|
| OR0			| rax		|
| OR1			| rdi		|
| OR2			| rsi		|
| OR3			| rdx		|

## All system calls

Each system call has a name and selection number. To select a system call, the
selection number must be loaded into ARSEL.

* 0x0 : `DBG_PRINTLN`
  * Write textual data to the kernel debug output (currently serial port)
  * AR0 = Log type (see kernel/include/kutil.h)
  * AR1 = Pointer to the null-terminated text buffer
* 0x20 : `PROG_TERM`
  * Request to be removed from scheduling and freed
  * AR0 = Termination code
