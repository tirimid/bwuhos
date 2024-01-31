void
_start(void)
{
	for (;;) {
		__asm__ volatile ("\tcli\n"
		                  "\thlt\n");
	}
}
