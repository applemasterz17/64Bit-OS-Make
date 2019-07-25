all: BootLoader.bin

BootLoader.bin: Bootloader.asm
	nasm -o BootLoader.bin BootLoader.asm

clean:
	rm -f Bootloader.bin
