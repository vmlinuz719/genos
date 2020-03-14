#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <deprecated/strlen.h>

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
#include <conshelper.h>
#include <handlers.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

InterruptDescriptor *idt;
SegmentDescriptor *gdt;

uint8_t heap[HEAP_SIZE] __attribute__((aligned(4096)));

void kmain(multiboot_info_t *mbd, unsigned int magic) {
	(void)mbd; (void)magic;

	termInit();
	termDisableCursor();

	kHeapInit(heap, HEAP_SIZE);
	
	initGDT(&gdt);

	i8259Remap(LEADER_PIC_REMAP, FOLLOWER_PIC_REMAP);
	i8259MaskAll();
	initIDT(&idt, 256);
	
	registerHandler(idt, 0x08, &_doublefault, true, false, 0, INTERRUPT_GATE);
	registerHandler(idt, 0x31, &_kbd, true, false, 0, INTERRUPT_GATE);
	i8259ClearMask(1);
	
	consoles[0] = (vgaConsoleDrv.init)(0);
	consHello(consoles[0]);
	
	uint16_t con1Port = 0x3F8;
	consoles[1] = (serialDrv.init)(&con1Port);
	consHello(consoles[1]);	
			
	asm("sti");
	
	consPrint(consoles[0], "Dumping keycodes now, ctrl to exit:\n");
	
	char spinner[] = "|/-\\";
	int spinnerIdx = 0;
	while (true) {
		consPutChar(consoles[1], '\b');
		consPutChar(consoles[1], spinner[spinnerIdx]);
		spinnerIdx = (spinnerIdx + 1) % 4;
	}
	
	consPutChar(consoles[1], '\n');
	consPrint(consoles[1], "...done!\n");
	
	consPutChar(consoles[0], '\n');
	consPrint(consoles[0], "Exiting.\n");

	asm("int $255");
}

