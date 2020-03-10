#if !defined (_CONSHELPER_H_)
#define _CONSHELPER_H_
#include <syslib/console.h>

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

#endif

