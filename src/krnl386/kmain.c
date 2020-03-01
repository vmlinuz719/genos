#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <fast-memcpy/memcpy.h>
#include <multiboot.h>
#include <vga.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

#define OSVERSION "GenOS a03022020"

void int2Hex(uint64_t x, char *buf, size_t size) {
	// expect buf to be n characters long - last one gets \0
	buf[size - 1] = '\0';

	uint64_t num = x;
	int i = size - 1;

	// (void)x;
	do {
		buf[--i] = "0123456789ABCDEF"[num % 16];
		num >>= 4;
	} while (num > 0 && i);

	while (--i >= 0) buf[i] = '0';
}

void kmain(multiboot_info_t *mbd, unsigned int magic) {
	termInit();
	termDisableCursor();
	termPrint(OSVERSION);
	termPrint("\n");
	termPrint("Copyright 2020 vmlinuz719. All rights reserved.\n\n");

	// (void)mbd;
	
	char magicBuf[9];

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		termPrint("Got multiboot info (0x");
		int2Hex(magic, magicBuf, 9);
		termPrint(magicBuf);
		termPrint(")\n");
	} else {
		termSetColor(vgaEntColor(vgaLRed, vgaBlue));
		termPrint
		("FATAL: GenOS requires a Multiboot-compliant bootloader!\n");
	}

	bool mmapValid = mbd->flags & MULTIBOOT_INFO_MEM_MAP;
	if (mmapValid) {
		termPrint("Multiboot memory map is complete (0x");
		int2Hex(mbd->flags, magicBuf, 9);
		termPrint(magicBuf);
		termPrint(")\n");
	}

	termSetColor(vgaEntColor(vgaLGreen, vgaBlue));
	termPrint("\n***All available functions completed***\n");
}

