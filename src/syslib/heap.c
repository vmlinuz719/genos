#include <syslib/heap.h>
// #include <vga.h>

HeapDescriptor *heapStart;
HeapDescriptor *heapEnd;
HeapDescriptor *lastFree;

void initHeapDescriptor(HeapDescriptor *hd,
	HeapDescriptor *prev, HeapDescriptor *next, bool usable) {
	hd->magic = HEAP_MAGIC;
	hd->prev = prev;
	hd->next = next;
	hd->used = false;
	hd->usable = usable;
}

void kHeapInit(uint8_t *h, size_t size) {
	HeapDescriptor *start = (HeapDescriptor *)h;
	HeapDescriptor *end =
		(HeapDescriptor *)(h + size - sizeof(HeapDescriptor));

	initHeapDescriptor(start, NULL, end, true);
	initHeapDescriptor(end, start, NULL, false);

	heapStart = start;
	heapEnd = end;
	lastFree = NULL;
}

size_t kDescriptorSize(HeapDescriptor *hd) {
	if (hd == NULL || hd->next == NULL) return 0;

	if (hd->magic == HEAP_MAGIC && hd->next->magic == HEAP_MAGIC
		&& hd->usable) {
		// termPrint("Success\n");
		return (uint8_t *)(hd->next)
			- (uint8_t *)hd;
	}

	return 0;
}

size_t kDescriptorRealSize(HeapDescriptor *hd) {
	size_t result = kDescriptorSize(hd);

	if (result == 0) return result;
	return result - sizeof(HeapDescriptor);
}

void *kMalloc(size_t size) {
	HeapDescriptor *hd = heapStart;

	while(hd != NULL &&
		(!(hd->usable)
		|| hd->used
		|| kDescriptorRealSize(hd) < size)) {
		if (hd->magic != HEAP_MAGIC) return NULL;
		hd = hd->next;
	}

	if (hd == NULL || hd->magic != HEAP_MAGIC) {
		// termPrint("kMalloc failure!\n");
		return NULL;
	}

	hd->used = true;

	if (kDescriptorRealSize(hd) > size + sizeof(HeapDescriptor)) {
		HeapDescriptor *new =
			(HeapDescriptor *)
			((uint8_t *)hd + size + sizeof(HeapDescriptor));
		hd->next->prev = new;
		initHeapDescriptor(new, hd, hd->next, true);
		hd->next = new;
		// termPrint("kMalloc split\n");
	}

	return (void *)((uint8_t *)hd + sizeof(HeapDescriptor));
}

void kFree(void *ptr) {
	HeapDescriptor *hd =
		(HeapDescriptor *)((uint8_t *)ptr - sizeof(HeapDescriptor));

	hd->used = false;

	HeapDescriptor *prevFree = hd;
	HeapDescriptor *nextFree = hd;

	while (prevFree->prev != NULL
		&& prevFree->prev->usable
		&& !(prevFree->prev->used)) {
		// termPrint("zip down\n");
		prevFree = prevFree->prev;
	}

	while (nextFree->next != NULL
		&& nextFree->next->usable
		&& !(nextFree->next->used)) {
		// termPrint("zip up\n");
		nextFree = nextFree->next;
	}

	if (prevFree != nextFree) {
		hd->magic = 0;

		prevFree->next = nextFree->next;
		nextFree->next->prev = prevFree;
	}
	lastFree = prevFree;
}
