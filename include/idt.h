#if !defined(_IDT_H_)
#define _IDT_H_
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct id {
	uint16_t offsetLow;
	uint16_t selector;
	uint8_t mustBeZero;
	uint8_t type;
	uint16_t offsetHigh;
} __attribute__((packed)) InterruptDescriptor;

typedef struct idtr {
	uint16_t limit;
	InterruptDescriptor *base;
} __attribute__((packed)) IDTRegister;

IDTRegister initIDT(InterruptDescriptor **idt, size_t size);
void registerHandler(InterruptDescriptor *idt, int vector, void *wrapper,
		bool present, bool storage, uint8_t dpl, uint8_t type);

#define TASK_GATE 5
#define INTERRUPT_GATE 14
#define TRAP_GATE 15

#endif
