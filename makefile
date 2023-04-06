C_FLAGS := -ffreestanding -nostdlib -nostdinc -fno-builtin -fstrength-reduce -fomit-frame-pointer -Iinclude -Ilib

TARGET := kernel.bin
BUILD_DIR := build
OBJECTS := $(BUILD_DIR)/main.o \
		$(BUILD_DIR)/gdt.o \
		$(BUILD_DIR)/idt.o \
		$(BUILD_DIR)/irq.o \
		$(BUILD_DIR)/io.o \
		$(BUILD_DIR)/isrs.o \
		$(BUILD_DIR)/timer.o \
		$(BUILD_DIR)/terminal.o \
		$(BUILD_DIR)/cmos.o \
		$(BUILD_DIR)/keyboard.o \
		$(BUILD_DIR)/memory.o \
		$(BUILD_DIR)/string.o \
		$(BUILD_DIR)/pmm.o \
		$(BUILD_DIR)/shell.o \
		$(BUILD_DIR)/start.o

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	i686-elf-gcc $^ -o $@ $(C_FLAGS) -T link.ld

$(BUILD_DIR)/%.o: %.c
	i686-elf-gcc -c $< -o $@ $(C_FLAGS)

$(BUILD_DIR)/%.o: cpu/%.c
	i686-elf-gcc -c $< -o $@ $(C_FLAGS)

$(BUILD_DIR)/%.o: devices/%.c
	i686-elf-gcc -c $< -o $@ $(C_FLAGS)

$(BUILD_DIR)/%.o: lib/memory/%.c
	i686-elf-gcc -c $< -o $@ $(C_FLAGS)

$(BUILD_DIR)/%.o: lib/string/%.c
	i686-elf-gcc -c $< -o $@ $(C_FLAGS)

$(BUILD_DIR)/%.o: %.asm
	nasm -f elf32 -o $@ $<

run: $(BUILD_DIR)/$(TARGET)
	qemu-system-x86_64w -kernel $< -m 128 -rtc base=localtime

clean:
	del /q .\build\*.*
