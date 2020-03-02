#if !defined(_HEAP_H_)
#define _HEAP_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define HEAP_SIZE 65536
#define HEAP_MAGIC 0xDEADBEEF

extern uint8_t heap[];

typedef struct hd {
	uint32_t magic;
	struct hd *prev;
	struct hd *next;
	bool used;
	bool usable;
} HeapDescriptor;

void kHeapInit(uint8_t *h, size_t size);
size_t kDescriptorSize(HeapDescriptor *hd);
size_t kDescriptorRealSize(HeapDescriptor *hd);
void *kMalloc(size_t size);
void kFree(void *ptr);

extern HeapDescriptor *heapStart;
extern HeapDescriptor *heapEnd;
extern HeapDescriptor *lastFree;

#endif

