#if !defined(_IRQ_H_)
#define _IRQ_H_
#include <stddef.h>
#include <stdint.h>

typedef struct is {
	uint32_t status;
	uint32_t EDI;
	uint32_t ESI;
	uint32_t EBP;
	uint32_t ESP;
	uint32_t EBX;
	uint32_t EDX;
	uint32_t ECX;
	uint32_t EAX;
	uint32_t error;
	uint32_t EIP;
	uint32_t CS;
	uint32_t EFLAGS;
} __attribute__((packed)) intState;

#endif
