#if !defined(_CONSOLE_H_)
#define _CONSOLE_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// This is the interface for a basic printer/console
// It implements printer-like functionality.
// TODO: keyboard/read support

typedef struct cons {
	void *data;
	void (*putchar)(void *, char);
	size_t (*print)(void *, char *);
	size_t (*nprint)(void *, char *, size_t);
	void (*pagebrk)(void *);
} Console;

void consPutChar(Console *cons, char c);
size_t consPrint(Console *cons, char *str);
size_t consNPrint(Console *cons, char *str, size_t size);
void consPageBrk(Console *cons);

/*
Example:
	Console *cons = vgaConsoleInit(0);
	
	consPrint(cons, "Hello from console driver land!\n");
	=>	(cons->print)(cons->data, "Hello from console driver land!\n");
*/

#endif

