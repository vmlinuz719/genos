#if !defined(_ADDM_H_)
#define _ADDM_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Device driver interface

typedef struct drv {
	void *(*init)(void *);
	bool (*destroy)(void *);
} Driver;

/*
Example:
	Console *cons = vgaConsoleInit(0);
	
	consPrint(cons, "Hello from console driver land!\n");
	=>	(cons->print)(cons->data, "Hello from console driver land!\n");
*/

#endif

