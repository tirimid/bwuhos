ECHO_COLOR="\033[1;35m"

echo -e "${ECHO_COLOR}[/] building dependency submodules\033[0m"
cd deps
./build.sh
cd ..

echo -e "${ECHO_COLOR}[/] building kernel\033[0m"
cd kernel
./build.sh
cd ..

echo -e "${ECHO_COLOR}[/] building base filesystem\033[0m"
mkdir -p root/boot
cp \
	deps/limine/limine.sys \
	deps/limine/limine-cd-efi.bin \
	deps/limine/limine-cd.bin \
	root/boot

cp kernel/kbin root/boot

xorriso \
	-as mkisofs \
	-b limine-cd.bin \
	-no-emul-boot \
	-boot-load-size 4 \
	-boot-info-table \
	--efi-boot limine-cd-efi.bin \
	-efi-boot-part \
	--efi-boot-image \
	--protective-msdos-label \
	-o bwuhos.iso root

echo -e "${ECHO_COLOR}[/] deploying bootloader\033[0m"
./deps/limine/limine-deploy bwuhos.iso
