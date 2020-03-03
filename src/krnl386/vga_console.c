#include <syslib/heap.h>
#include <syslib/console.h>
#include <addm.h>
#include <vga.h>
#include <vga_console.h>

// Singleton driver for VGA console
// Essentially a shim to fit the vga console into our ADDM console interface

bool initialized;

typedef struct vgacons {
	void *address;
} VGAConsoleData;

void vgaConsolePutChar(void *data, char c) {
	(void)data;
	
	termPutChar(c);
}

size_t vgaConsolePrint(void *data, char *str) {
	(void)data;
	
	return termPrint(str);
}

size_t vgaConsoleNPrint(void *data, char *str, size_t size) {
	(void)data;
	
	termWrite(str, size);
	return size;
}

void vgaConsolePageBrk(void *data) {
	(void)data;
	
	termInit();
}

void *initVGAConsole(void *unused) {
	(void)unused;
	
	void *result = NULL;
	if (initialized) return NULL;
	
	termInit();
	
	initialized = true;
	
	result = kMalloc(sizeof(Console));
	if (result == NULL) return NULL;
	Console *cons = (Console *)result;
	cons->putchar = vgaConsolePutChar;
	cons->print = vgaConsolePrint;
	cons->nprint = vgaConsoleNPrint;
	cons->pagebrk = vgaConsolePageBrk;
	
	return result;
}

Driver vgaConsoleDrv = {initVGAConsole, NULL};

