qemu-system-x86_64 \
	-cpu qemu64 \
	-m 4G \
	--no-reboot \
	-d int -D emu_log.txt \
	-s -S \
	-serial file:/dev/stdout \
	-hda bwuhos.iso
