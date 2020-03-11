#if !defined(_VGA_H_)
#define _VGA_H_

#include <stddef.h>
#include <stdint.h>
#include <deprecated/strlen.h>

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_ADDRESS	0xB8000

typedef enum vc {
        vgaBlack        = 0,
        vgaBlue         = 1,
        vgaGreen        = 2,
        vgaCyan         = 3,
        vgaRed          = 4,
        vgaMagenta      = 5,
        vgaBrown        = 6,
        vgaLGray        = 7,
        vgaDGray        = 8,
        vgaLBlue        = 9,
        vgaLGreen       = 10,
        vgaLCyan        = 11,
        vgaLRed         = 12,
        vgaLMagenta     = 13,
        vgaLBrown       = 14,
        vgaWhite        = 15,
} VGAColor;

static inline uint8_t vgaEntColor(VGAColor fg, VGAColor bg) {
        return fg | bg << 4;
}

static inline uint16_t vgaEnt(unsigned char c, uint8_t color) {
        return (uint16_t)c | (uint16_t)color << 8;
}

void termInit();
void termInitColor(uint8_t color);
void termSetColor(uint8_t color);
void termEntPut(char c, uint8_t color, size_t x, size_t y);
void termPutChar(char c);
void termWrite(const char* buf, size_t size);
size_t termPrint(const char* str);
void termEnableCursor(uint8_t cursorStart, uint8_t cursorEnd);
void termDisableCursor();

static inline void termHex(int x) {
	char buf[9];
	int2Hex(x, buf, 9);
	// consPrint(cons, "(0x");
	termPrint(buf);
	// consPrint(cons, ") ");
}

static inline void termHexShort(int x) {
	char buf[5];
	int2Hex(x, buf, 5);
	// consPrint(cons, "(0x");
	termPrint(buf);
	// consPrint(cons, ") ");
}

#endif
