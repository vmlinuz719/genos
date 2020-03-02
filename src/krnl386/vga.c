#include <vga.h>
#include <port.h>
#include <deprecated/strlen.h>
#include <fast-memcpy/memcpy.h>

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

size_t termPrint(const char* str) {
		size_t size = deprecatedStrlen(str);
        termWrite(str, size);
        return size;
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

