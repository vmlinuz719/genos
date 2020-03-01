CC=i686-elf-gcc
AS=i686-elf-as
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
QEMU=qemu-system-i386
QEMUFLAGS=

KRNL386=src/krnl386

kmain.o:
	$(CC) $(CFLAGS) -c $(KRNL386)/kmain.c -o $(KRNL386)/kmain.o

bootstrap.o:
	$(AS) $(KRNL386)/bootstrap.s -o $(KRNL386)/bootstrap.o

krnl386.sys: kmain.o bootstrap.o
	$(CC) $(LDFLAGS) $(KRNL386)/bootstrap.o $(KRNL386)/kmain.o \
	-T link/krnl386.ld -o bin/krnl386.sys

test-krnl386: krnl386.sys
	$(QEMU) $(QEMUFLAGS) -kernel bin/krnl386.sys

clean:
	rm src/*/*.o
	rm bin/*

