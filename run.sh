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

echo -e "${ECHO_COLOR}[/] running bwuhos\033[0m"
case $ARCH in
	x86_64)
		qemu-system-x86_64 \
			-cpu qemu64 \
			-m 4G \
			--no-reboot \
			-d int -D emu_log.txt \
			-s -S \
			-serial file:/dev/stdout \
			-hda bwuhos.iso
esac

# QEmu arguments for AHCI SATA disk emulation:
# -drive id=disk,file=bwuhos.iso,if=none \
# -device ahci,id=ahci \
# -device ide-hd,drive=disk,bus=ahci.0
