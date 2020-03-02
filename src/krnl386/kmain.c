#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <deprecated/strlen.h>

#include <fast-memcpy/memcpy.h>
#include <multiboot.h>
#include <vga.h>
#include <syslib/heap.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

#define OSVERSION "GenOS a03022020"

uint8_t heap[HEAP_SIZE] __attribute__((aligned(4096)));

void intStatus(int x) {
	char buf[9];
	int2Hex(x, buf, 9);
	termPrint("(0x");
	termPrint(buf);
	termPrint(") ");
}

void kmain(multiboot_info_t *mbd, unsigned int magic) {
	termInit();
	termDisableCursor();
	termPrint(OSVERSION);
	termPrint("\n");
	termPrint("Copyright 2020 vmlinuz719. All rights reserved.\n\n");

	// (void)mbd;
	
	// intStatus(magic);
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		termPrint("Got multiboot info\n");
	} else {
		termSetColor(vgaEntColor(vgaLRed, vgaBlue));
		termPrint
		("FATAL: requires Multiboot!\n");
		goto end;
	}

	// intStatus(mbd->flags);
	bool mmapValid = mbd->flags & MULTIBOOT_INFO_MEM_MAP;
	if (mmapValid) {
		termPrint("Mmap valid\n\n");
		intStatus(mbd->mmap_addr);
		intStatus(mbd->mmap_length);
		termPrint("Memory map base, length\n");

		multiboot_memory_map_t *entry =
			(multiboot_memory_map_t *)mbd->mmap_addr;
		while (entry < (multiboot_memory_map_t *)
			mbd->mmap_addr + mbd->mmap_length) {
			if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
				intStatus(entry->addr);
				intStatus(entry->len);
				termPrint("Usable memory base, length\n");
			}
			entry = (multiboot_memory_map_t *)((unsigned int) entry
				+ entry->size + sizeof(entry->size));
		}
	} else {
		termPrint("FATAL: Mmap invalid!\n");
		goto end;
	}

	termPrint("\nInitializing heap...\n\n");

	intStatus((int)heap);
	intStatus((int)HEAP_SIZE);
	termPrint("Heap base, length\n");

	kHeapInit(heap, HEAP_SIZE);

	intStatus((int)heapStart);
	intStatus((int)heapEnd);
	termPrint("Heap start, end descriptors\n");

	intStatus(kDescriptorRealSize(heapStart));
	termPrint("             Real heap size\n\n");

	termPrint("Dummy workload: testing kMalloc()...\n\n");

	int *x = kMalloc(sizeof(int));
	intStatus((int)x);
	termPrint("             Mallocated integer address\n");

	int *y = kMalloc(sizeof(int));
	intStatus((int)y);
	termPrint("             Mallocated integer address\n");

	int *z = kMalloc(sizeof(int));
	intStatus((int)z);
	termPrint("             Mallocated integer address\n");

end:
	termSetColor(vgaEntColor(vgaLGreen, vgaBlue));
	termPrint("\n***All available functions completed***\n");

	// for (;;) termPrint("ANIMOSITISOMIN");
}

