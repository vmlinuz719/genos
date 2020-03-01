#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_ADDRESS	0xB8000

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
	termColor = vgaEntColor(vgaLGray, vgaBlue);
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
	termEntPut(c, termColor, termCol, termRow);
	if (++termCol == VGA_WIDTH) {
		termCol = 0;
		if (++termRow == VGA_HEIGHT) {
			// TODO: scroll
			termRow = 0;
		}
	}
}

void termWrite(const char* buf, size_t size) {
	for (size_t i = 0; i < size; i++)
		termPutChar(buf[i]);
}

void termPrint(const char* str) {
	termWrite(str, deprecatedStrlen(str));
}

void kmain(void) {
	termInit();
	termPrint("BSOD");
}

