qemu-system-x86_64 \
	-cpu qemu64 \
	-m 4G \
	--no-reboot \
	-d int -D emu_log.txt \
	-s -S \
	-serial file:/dev/stdout \
	-hda bwuhos.iso

# use for AHCI SATA disk emulation.
#	-drive id=disk,file=bwuhos.iso,if=none \
#	-device ahci,id=ahci \
#	-device ide-hd,drive=disk,bus=ahci.0
