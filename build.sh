ECHO_COLOR="\033[1;32m"

echo -e "${ECHO_COLOR}[/] building dependency submodules\033[0m"
cd deps
./build.sh
cd ..

echo -e "${ECHO_COLOR}[/] building kernel\033[0m"
cd kernel
./build.sh
if [ $? -ne 0 ]
then
	echo -e "${ECHO_COLOR}[/] kernel build failed\033[0m"
	exit 1
fi
cd ..

echo -e "${ECHO_COLOR}[/] building base filesystem\033[0m"
mkdir -p root/boot
cp \
	deps/limine/limine-bios.sys \
	root/boot

cp kernel/kbin root/boot

xorriso \
	-as mkisofs \
	-b boot/limine-bios-cd.bin \
	-no-emul-boot \
	-boot-load-size 4 \
	-boot-info-table \
	--efi-boot boot/limine-uefi-cd.bin \
	-efi-boot-part \
	--efi-boot-image \
	--protective-msdos-label \
	-o bwuhos.iso root 2> /dev/null

echo -e "${ECHO_COLOR}[/] deploying bootloader\033[0m"
./deps/limine/limine bios-install bwuhos.iso 2> /dev/null
