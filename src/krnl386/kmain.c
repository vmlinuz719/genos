#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <deprecated/strlen.h>

#include <fast-memcpy/memcpy.h>
#include <multiboot.h>
#include <vga.h>
#include <syslib/heap.h>
#include <idt.h>
#include <gdt.h>

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

bool dumpMmap(multiboot_info_t *mbd) {
	bool mmapValid = mbd->flags & MULTIBOOT_INFO_MEM_MAP;
        if (mmapValid) {
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
        }

	return mmapValid;
}

void initGDT() {
	GDTRegister gdtr;
        SegmentDescriptor *gdt = kMalloc(sizeof(SegmentDescriptor) * 8);
        intStatus((int)gdt);
        termPrint("Global Descriptor Table address\n");

        gdt[0] = createDescriptor(0, 0, 0);
        gdt[1] = createDescriptor(0, 0xFFFFF, (GDT_CODE_PL0));
        gdt[2] = createDescriptor(0, 0xFFFFF, (GDT_DATA_PL0));
        gdt[3] = createDescriptor(0, 0xFFFFF, (GDT_CODE_PL3));
        gdt[4] = createDescriptor(0, 0xFFFFF, (GDT_DATA_PL3));

        gdtr.base = gdt;
        gdtr.size = 32 * 8 - 1;

        __asm__ __volatile__ ("lgdt (%0)": : "r" (&gdtr));
        _flush_gdt();
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
	if (!dumpMmap(mbd)) {
		termPrint("FATAL: Mmap invalid!\n");
		goto end;
	}

	termPrint("\n");

	kHeapInit(heap, HEAP_SIZE);

	intStatus(kDescriptorRealSize(heapStart));
	termPrint("Real heap size\n\n");

	initGDT();

end:
	termSetColor(vgaEntColor(vgaLGreen, vgaBlue));
	termPrint("\n***All available functions completed***");

	// for (;;) termPrint("ANIMOSITISOMIN");
}

