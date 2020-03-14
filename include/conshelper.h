#if !defined (_CONSHELPER_H_)
#define _CONSHELPER_H_
#include <syslib/console.h>
#include <deprecated/strlen.h>
#include <syslib/genos.h>

static inline void consIntStatus(Console *cons, int x) {
	char buf[9];
	int2Hex(x, buf, 9);
	consPrint(cons, "(0x");
	consPrint(cons, buf);
	consPrint(cons, ") ");
}

static inline void consHex(Console *cons, int x) {
	char buf[9];
	int2Hex(x, buf, 9);
	// consPrint(cons, "(0x");
	consPrint(cons, buf);
	// consPrint(cons, ") ");
}

static inline void consHexByte(Console *cons, char x) {
	char buf[3];
	int2Hex(x, buf, 3);
	// consPrint(cons, "(0x");
	consPrint(cons, buf);
	// consPrint(cons, ") ");
}

static inline void consBool(Console *cons, bool b) {
	if (b) consPrint(cons, "true");
	else consPrint(cons, "false");
}

static inline void consHello(Console *cons) {
	consPutChar(cons, '\n');
	consPrint(cons, "Console: ");
	consIntStatus(cons, (int)cons);
	consPrint(cons, "\n\n");
	
	consPrint(cons, OSVERSION);
	consPutChar(cons, '\n');
	consPrint(cons, "(C) 2020 vmlinuz719. All rights reserved.\n\n");
}

#endif

