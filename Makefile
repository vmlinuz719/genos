CC=i686-elf-gcc
AS=i686-elf-as
INCLUDE=include
CFLAGS=-I$(INCLUDE) -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
QEMU=qemu-system-i386
QEMUFLAGS=-m 1G -serial stdio

KRNL386=src/krnl386
MEMCPY=src/fast-memcpy
DEPRECATED=src/deprecated
SYSLIB=src/syslib

OBJDIR=build
LDSCRIPTDIR=link
BINDIR=bin

gdt.o:
	$(CC) $(CFLAGS) -c $(KRNL386)/gdt.c -o $(OBJDIR)/gdt.o

idt.o:
	$(CC) $(CFLAGS) -c $(KRNL386)/idt.c -o $(OBJDIR)/idt.o

8259.o:
	$(CC) $(CFLAGS) -c $(KRNL386)/8259.c -o $(OBJDIR)/8259.o

console.o:
	$(CC) $(CFLAGS) -c $(SYSLIB)/console.c -o $(OBJDIR)/console.o

heap.o:
	$(CC) $(CFLAGS) -c $(SYSLIB)/heap.c -o $(OBJDIR)/heap.o

strlen.o:
	$(CC) $(CFLAGS) -c $(DEPRECATED)/strlen.c -o $(OBJDIR)/strlen.o

vga.o:
	$(CC) $(CFLAGS) -c $(KRNL386)/vga.c -o $(OBJDIR)/vga.o

vga_console.o:
	$(CC) $(CFLAGS) -Werror -c \
	$(KRNL386)/vga_console.c -o $(OBJDIR)/vga_console.o

serial.o:
	$(CC) $(CFLAGS) -Werror -c \
        $(KRNL386)/serial.c -o $(OBJDIR)/serial.o

memcpy.o:
	$(CC) $(CFLAGS) -c $(MEMCPY)/memcpy.c -o $(OBJDIR)/memcpy.o

kmain.o:
	$(CC) $(CFLAGS) -Werror -c $(KRNL386)/kmain.c -o $(OBJDIR)/kmain.o

bootstrap.o:
	$(AS) $(KRNL386)/bootstrap.s -o $(OBJDIR)/bootstrap.o

handlers.o:
	$(AS) $(KRNL386)/handlers.s -o $(OBJDIR)/handlers.o
	$(CC) $(CFLAGS) -Werror -c \
	$(KRNL386)/handlers.c -o $(OBJDIR)/handlers_c.o

krnl386.sys: kmain.o bootstrap.o memcpy.o vga.o strlen.o heap.o console.o \
	vga_console.o serial.o 8259.o gdt.o idt.o handlers.o
	$(CC) $(LDFLAGS) $(OBJDIR)/bootstrap.o $(OBJDIR)/kmain.o \
	$(OBJDIR)/memcpy.o $(OBJDIR)/vga.o $(OBJDIR)/strlen.o \
	$(OBJDIR)/heap.o $(OBJDIR)/console.o $(OBJDIR)/vga_console.o \
	$(OBJDIR)/serial.o $(OBJDIR)/gdt.o $(OBJDIR)/8259.o \
	$(OBJDIR)/handlers.o $(OBJDIR)/handlers_c.o $(OBJDIR)/idt.o \
	-T $(LDSCRIPTDIR)/krnl386.ld -o $(BINDIR)/krnl386.sys

test-krnl386: krnl386.sys
	$(QEMU) $(QEMUFLAGS) -kernel $(BINDIR)/krnl386.sys

clean:
	rm $(OBJDIR)/*.o
	rm $(BINDIR)/*

