// quick and dirty program to test code loading.

void
_start(void)
{
	for (;;) {
		__asm__ volatile("\tcli\n"
		                 "\thlt\n");
	}
}
