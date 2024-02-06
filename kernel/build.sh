ECHO_COLOR="\033[1;32m"

echo -e "${ECHO_COLOR}[/kernel] building C sources\033[0m"
mincbuild -rv build-conf/mcb_c.conf
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to build C sources\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] building assembly sources\033[0m"
mincbuild -rv build-conf/mcb_S.conf
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to build assembly sources\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] linking kernel binary\033[0m"
x86_64-elf-gcc \
	-Tkbin.ld \
	-ffreestanding \
	-nostdlib \
	-lgcc \
	-z max-page-size=4096 \
	-o kbin \
	-g \
	$(find lib -name "*.o")
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to link kernel binary\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] building kernel debug file\033[0m"
objcopy --only-keep-debug kbin kbin.debug
objcopy --strip-debug kbin

echo -e "${ECHO_COLOR}[/kernel] applying final changes\033[0m"
chmod -x kbin
