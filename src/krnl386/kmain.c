#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <fast-memcpy/memcpy.h>
#include <multiboot.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_ADDRESS	0xB8000

#define OSVERSION "GenOS a03022020"

typedef enum vc {
	vgaBlack	= 0,
	vgaBlue		= 1,
	vgaGreen	= 2,
	vgaCyan		= 3,
	vgaRed		= 4,
	vgaMagenta	= 5,
	vgaBrown	= 6,
	vgaLGray	= 7,
	vgaDGray	= 8,
	vgaLBlue	= 9,
	vgaLGreen	= 10,
	vgaLCyan	= 11,
	vgaLRed		= 12,
	vgaLMagenta	= 13,
	vgaLBrown	= 14,
	vgaWhite	= 15,
} VGAColor;

static inline uint8_t vgaEntColor(VGAColor fg, VGAColor bg) {
	return fg | bg << 4;
}

static inline uint16_t vgaEnt(unsigned char c, uint8_t color) {
	return (uint16_t)c | (uint16_t)color << 8;
}

size_t deprecatedStrlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

size_t termRow, termCol;
uint8_t termColor;
uint16_t* termBuf;

void termInit() {
	termRow = termCol = 0;
	termColor = vgaEntColor(vgaWhite, vgaBlue);
	termBuf = (uint16_t *)VGA_ADDRESS;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t i = y * VGA_WIDTH + x;
			termBuf[i] = vgaEnt(' ', termColor);
		}
	}
}

void termSetColor(uint8_t color) {
	termColor = color;
}

void termEntPut(char c, uint8_t color, size_t x, size_t y) {
	const size_t i = y * VGA_WIDTH + x;
	termBuf[i] = vgaEnt(c, color);
}

void termPutChar(char c) {
	if (c != '\n')
		termEntPut(c, termColor, termCol, termRow);
	if (++termCol == VGA_WIDTH || c == '\n') {
		termCol = 0;
		termRow++;
	}
	if (termRow == VGA_HEIGHT) {
		termRow--;
		memcpy(termBuf, (termBuf + VGA_WIDTH),
				(VGA_WIDTH * (VGA_HEIGHT - 1))
				* sizeof(uint16_t));
		for (size_t x = 0; x < VGA_WIDTH; x++)
			termEntPut(' ', termColor, x, VGA_HEIGHT - 1);
	}
}

void termWrite(const char* buf, size_t size) {
	for (size_t i = 0; i < size; i++)
		termPutChar(buf[i]);
}

void termPrint(const char* str) {
	termWrite(str, deprecatedStrlen(str));
}

static inline void outb(uint16_t port, uint8_t val) {
	    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile ( "inb %1, %0"
			: "=a"(ret)
			: "Nd"(port) );
	return ret;
}

void termEnableCursor(uint8_t cursorStart, uint8_t cursorEnd) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursorStart);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursorEnd);
}

void termDisableCursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

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
		termPrint("FATAL: GenOS requires a Multiboot-compliant bootloader!\n");
	}

	termSetColor(vgaEntColor(vgaLGreen, vgaBlue));
	termPrint("\n***All available functions completed***\n");
}

