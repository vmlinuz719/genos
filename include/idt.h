#if !defined(_IDT_H_)
#define _IDT_H_
#include <stddef.h>
#include <stdint.h>

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

#endif
