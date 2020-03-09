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
#include <addm.h>
#include <syslib/console.h>
#include <vga_console.h>
#include <serial.h>
#include <8259.h>
#include <port.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

#define OSVERSION "GenOS a03022020"

uint8_t heap[HEAP_SIZE] __attribute__((aligned(4096)));



static inline void intStatus(int x) {
	char buf[9];
	int2Hex(x, buf, 9);
	termPrint("(0x");
	termPrint(buf);
	termPrint(") ");
}

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

static inline void consBool(Console *cons, bool b) {
	if (b) consPrint(cons, "true");
	else consPrint(cons, "false");
}

void kbd() {
	//consPrint(consoles[0], "Interrupted!\n");
	unsigned char code = inb(0x60);
	consPutChar(consoles[0], code);
	i8259EOI(0);
}

extern void *kbdWrapper;

void kmain(multiboot_info_t *mbd, unsigned int magic) {
	termInit();
	termDisableCursor();

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
	if (!(mbd->flags & MULTIBOOT_INFO_MEM_MAP)) {
		termSetColor(vgaEntColor(vgaLRed, vgaBlue));
		termPrint("FATAL: Mmap invalid!\n");
		goto end;
	}

	kHeapInit(heap, HEAP_SIZE);

	initGDT();
	
	consoles[0] = (vgaConsoleDrv.init)(0);
	consPutChar(consoles[0], '\n');
	consPrint(consoles[0], "CON0: ");
	consIntStatus(consoles[0], (int)consoles[0]);
	consPrint(consoles[0], "\n\n");
	
	uint16_t con1Port = 0x3F8;
	
	consoles[1] = (serialDrv.init)(&con1Port);
	consPutChar(consoles[1], '\n');
	consPrint(consoles[1], "CON1: ");
	consIntStatus(consoles[1], (int)consoles[1]);
	consPrint(consoles[1], "\n\n");
	
	consPrint(consoles[0], OSVERSION);
	consPutChar(consoles[0], '\n');
	consPrint(consoles[0], "Copyright 2020 vmlinuz719. All rights reserved.\n");
	
	consPrint(consoles[1], "Hello from hardware abstraction land!\n");
	
	consPrint(consoles[1], "Remapping PIC and masking all.\n");
	i8259Remap(LEADER_PIC_REMAP, FOLLOWER_PIC_REMAP);
	i8259MaskAll();
	
	consPrint(consoles[1], "Allocating IDT.\n");
	InterruptDescriptor *idt = kMalloc(sizeof(SegmentDescriptor) * 256);
	consPrint(consoles[1], "Allocated ");
	consIntStatus(consoles[1], (int)idt);
	consPrint(consoles[1], "\n");
	
	consPrint(consoles[1], "Loading IDT.\n");
	IDTRegister idtr;
	idtr.limit = sizeof(SegmentDescriptor) * 256;
	idtr.base = idt;
	
	consPrint(consoles[1], "Survived, setting up keyboard irq.\n");
	unsigned long kbdcall = (unsigned long)&kbdWrapper;

	idt[0x31].offsetLow = kbdcall & 0xffff;
	idt[0x31].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	idt[0x31].mustBeZero = 0;
	idt[0x31].type = 0x8e; /* INTERRUPT_GATE */
	idt[0x31].offsetHigh = (kbdcall & 0xffff0000) >> 16;
	
	i8259ClearMask(1);
	__asm__ __volatile__ ("lidt (%0)": : "r" (&idtr));
	asm("sti");
	for (;;) {
		for (int x = 0; x < 100000000; x++) {}
		consPrint(consoles[1], "running ");
	}

end:
	termSetColor(vgaEntColor(vgaLGreen, vgaBlue));
	termPrint("\n*** All available functions completed ***");

	// for (;;) termPrint("ANIMOSITISOMIN");
}

