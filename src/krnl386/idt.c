#include <idt.h>
#include <syslib/heap.h>
#include <stdint.h>
#include <stdbool.h>

IDTRegister initIDT(InterruptDescriptor **idt, size_t size) {
	*idt = kMalloc(sizeof(InterruptDescriptor) * size);
	IDTRegister idtr;
	idtr.limit = sizeof(InterruptDescriptor) * size;
	idtr.base = *idt;
	
	__asm__ __volatile__ ("lidt (%0)": : "r" (&idtr));
	return idtr;
}

void registerHandler(InterruptDescriptor *idt, int vector, void *wrapper,
		bool present, bool storage, uint8_t dpl, uint8_t type) {
	
	unsigned long dfcall = (unsigned long)wrapper;
	
	uint8_t t;
	
	t |= (!!present & 1) << 7;
	t |= (dpl & 3) << 5;
	t |= (storage & 1) << 4;
	t |= type & 0x0F;
	
	idt[vector].offsetLow = dfcall & 0xffff;
	idt[vector].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	idt[vector].mustBeZero = 0;
	idt[vector].type = t; /* INTERRUPT_GATE */
	idt[vector].offsetHigh = (dfcall & 0xffff0000) >> 16;
}
