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

void kmain(multiboot_info_t *mbd, unsigned int magic) {
	termInit();
	termDisableCursor();
	termPrint(OSVERSION);
	termPrint("\n");
	termPrint("Copyright 2020 vmlinuz719. All rights reserved.\n\n");

	(void)mbd;
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		termPrint("Got multiboot info (0x2BADB002)\n");
	} else {
		termSetColor(vgaEntColor(vgaLRed, vgaBlue));
		termPrint
		("FATAL: GenOS requires a Multiboot-compliant bootloader!\n");
	}

	termSetColor(vgaEntColor(vgaLGreen, vgaBlue));
	termPrint("\n***All available functions completed***\n");
}

