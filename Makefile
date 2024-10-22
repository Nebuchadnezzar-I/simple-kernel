# Makefile for building the kernel

ASM_SRC = src/kernel.asm
C_SRC = src/kernel.c
LINKER_SCRIPT = src/link.ld
KERNEL_BIN = kernel
ASM_OBJ = kasm.o
C_OBJ = kc.o

all: $(KERNEL_BIN)

$(ASM_OBJ): $(ASM_SRC)
	nasm -f elf32 $(ASM_SRC) -o $(ASM_OBJ)

$(C_OBJ): $(C_SRC)
	gcc -m32 -c $(C_SRC) -o $(C_OBJ)

$(KERNEL_BIN): $(ASM_OBJ) $(C_OBJ) $(LINKER_SCRIPT)
	ld -m elf_i386 -T $(LINKER_SCRIPT) -o $(KERNEL_BIN) $(ASM_OBJ) $(C_OBJ)

clean:
	rm -f $(ASM_OBJ) $(C_OBJ) $(KERNEL_BIN)

run: all
	qemu-system-i386 -kernel $(KERNEL_BIN)

.PHONY: all clean run

