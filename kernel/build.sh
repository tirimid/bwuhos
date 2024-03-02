#!/bin/bash

ECHO_COLOR="\033[1;32m"

if [ $# -ne 1 ]
then
	echo -e "${ECHO_COLOR}[/kernel] wrong usage\033[0m"
	exit 1
fi

ARCH="$1"

case $ARCH in
	x86_64)
		;;
	*)
		echo -e "${ECHO_COLOR}[/kernel] unsupported arch: $ARCH\033[0m"
		exit 1
		;;
esac

echo -e "${ECHO_COLOR}[/kernel] building arch C sources\033[0m"
mincbuild -v conf/$ARCH/mcb_arch_c.conf
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to build arch C sources\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] building arch assembly sources\033[0m"
mincbuild -v conf/$ARCH/mcb_arch_S.conf
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to build arch assembly sources\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] building kernel C sources\033[0m"
mincbuild -v conf/$ARCH/mcb_kernel_c.conf
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to build kernel C sources\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] building kernel assembly sources\033[0m"
mincbuild -v conf/$ARCH/mcb_kernel_S.conf
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to build kernel assembly sources\033[0m"
	exit 1
fi

OBJS="$(find kernel/lib -name '*.o') $(find arch/$ARCH/lib -name '*.o')"

echo -e "${ECHO_COLOR}[/kernel] linking kernel binary\033[0m"
case $ARCH in
	x86_64)
		/opt/cross/bin/x86_64-elf-gcc \
			-Tconf/$ARCH/kbin.ld \
			-ffreestanding \
			-nostdlib \
			-lgcc \
			-z max-page-size=4096 \
			-o kbin \
			-g \
			$OBJS
		;;
esac
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/kernel] failed to link kernel binary\033[0m"
	exit 1
fi

echo -e "${ECHO_COLOR}[/kernel] building kernel debug file\033[0m"
objcopy --only-keep-debug kbin kbin.debug
objcopy --strip-debug kbin
