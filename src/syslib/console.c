#include <syslib/console.h>

void consPutChar(Console *cons, char c) {
	return (cons->putchar)(cons->data, c);
}

size_t consPrint(Console *cons, char *str) {
	return (cons->print)(cons->data, str);
}

size_t consNPrint(Console *cons, char *str, size_t size) {
	return (cons->nprint)(cons->data, str, size);
}

void consPageBrk(Console *cons) {
	(cons->pagebrk)(cons->data);
}

