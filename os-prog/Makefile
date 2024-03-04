.PHONY: all clean

CC := /opt/cross/bin/x86_64-elf-gcc
LD := /opt/cross/bin/x86_64-elf-gcc
CFLAGS := -std=c99 -pedantic -ffreestanding -fPIC -nostdlib -mno-red-zone
LDFLAGS := -ffreestanding -nostdlib -lgcc -z max-page-size=4096

SRC_DIR := src
LIB_DIR := lib
BIN_DIR := bin

PROGS := $(BIN_DIR)/bwush

all: $(PROGS)

clean:
	rm -rf $(LIB_DIR) $(BIN_DIR)

$(BIN_DIR)/%: $(LIB_DIR)/%.c.o
	@ mkdir -p $@
	@ rmdir $@
	$(LD) $(LDFLAGS) -o $@ $<

$(LIB_DIR)/%.c.o: $(SRC_DIR)/%.c
	@ mkdir -p $@
	@ rmdir $@
	$(CC) $(CFLAGS) -o $@ -c $<
