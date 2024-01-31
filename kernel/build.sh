ECHO_COLOR="\033[1;35m"

echo -e "${ECHO_COLOR}[/kernel] building C sources\033[0m"
mincbuild build-conf/mcb_c.conf

echo -e "${ECHO_COLOR}[/kernel] building assembly sources\033[0m"
mincbuild build-conf/mcb_s.conf

echo -e "${ECHO_COLOR}[/kernel] linking kernel binary\033[0m"
x86_64-elf-gcc \
	-Tkbin.ld \
	-ffreestanding \
	-nostdlib \
	-lgcc \
	-z max-page-size=4096 \
	-o kbin \
	$(find lib -name "*.o")

echo -e "${ECHO_COLOR}[/kernel] applying final changes\033[0m"
chmod -x kbin
