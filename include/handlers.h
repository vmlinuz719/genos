#if !defined (_HANDLERS_H_)
#define _HANDLERS_H_

#include <irq.h>

extern void *_kbd;
extern void *_doublefault;

void kbd(intState state);
void bugcheck(intState state);

#endif

