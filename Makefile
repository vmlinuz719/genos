CC=i686-elf-gcc
AS=i686-elf-as
INCLUDE=include
CFLAGS=-I$(INCLUDE) -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
QEMU=qemu-system-i386
QEMUFLAGS=

KRNL386=src/krnl386
MEMCPY=src/fast-memcpy

memcpy.o:
	$(CC) $(CFLAGS) -c $(MEMCPY)/memcpy.c -o $(MEMCPY)/memcpy.o

kmain.o:
	$(CC) $(CFLAGS) -Werror -c $(KRNL386)/kmain.c -o $(KRNL386)/kmain.o

bootstrap.o:
	$(AS) $(KRNL386)/bootstrap.s -o $(KRNL386)/bootstrap.o

krnl386.sys: kmain.o bootstrap.o memcpy.o
	$(CC) $(LDFLAGS) $(KRNL386)/bootstrap.o $(KRNL386)/kmain.o \
	$(MEMCPY)/memcpy.o \
	-T link/krnl386.ld -o bin/krnl386.sys

test-krnl386: krnl386.sys
	$(QEMU) $(QEMUFLAGS) -kernel bin/krnl386.sys

clean:
	rm src/*/*.o
	rm bin/*

