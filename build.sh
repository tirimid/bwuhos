#!/bin/bash

ECHO_COLOR="\033[1;32m"

if [ $# -ne 1 ]
then
	echo -e "${ECHO_COLOR}[/] wrong usage\033[0m"
	exit 1
fi

ARCH="$1"

case $ARCH in
	x86_64)
		;;
	*)
		echo -e "${ECHO_COLOR}[/] unsupported arch: $ARCH\033[0m"
		exit 1
		;;
esac

echo -e "${ECHO_COLOR}[/] building dependency submodules\033[0m"
cd deps
./build.sh
cd ..

echo -e "${ECHO_COLOR}[/] building kernel\033[0m"
cd kernel
./build.sh $ARCH
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/] kernel build failed\033[0m"
	exit 1
fi
cd ..

echo -e "${ECHO_COLOR}[/] building base filesystem\033[0m"
mkdir -p root/boot
cp \
	deps/limine/limine-bios-cd.bin \
	deps/limine/limine-uefi-cd.bin \
	deps/limine/limine-bios.sys \
	root/boot

cp kernel/kbin root/boot

echo -e "${ECHO_COLOR}[/] creating disk image\033[0m"
dd if=/dev/zero of=bwuhos-fs.img bs=1M count=50 2> /dev/null
mformat -i bwuhos-fs.img ::
mcopy -i bwuhos-fs.img root/* ::
fs2di --out=bwuhos.img bwuhos-fs.img boot 6

./deps/limine/limine bios-install bwuhos.img --quiet
