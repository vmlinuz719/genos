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
#include <irq.h>
#include <addm.h>
#include <syslib/console.h>
#include <vga_console.h>
#include <serial.h>
#include <8259.h>
#include <port.h>
#include <conshelper.h>

#if defined(__linux__)
#error "ERROR: targeting linux (don't even try)"
#endif

#if !defined(__i386__)
#error "ERROR: targeting wrong architecture (needs x86)"
#endif

#define OSVERSION "GenOS a03112020"

uint8_t heap[HEAP_SIZE] __attribute__((aligned(4096)));

bool done;

void kbd(intState state) {
	//consPrint(consoles[0], "Interrupted!\n");
	unsigned char code = inb(0x60);
	if (code == 0x1d) done = true;
	else if (state.status == 0xDEADBEEF) {
		consHexByte(consoles[0], code);
		consPutChar(consoles[0], ' ');
	}
	i8259EOI(0);
}

void bugcheck(intState state) {
	(void)state;
	termInitColor(vgaEntColor(vgaLRed, vgaBlack));
	termPrint("\n*** ");
	termPrint(OSVERSION);
	termPrint(" BUGCHECK ***\n\n");
	termPrint("Sorry, a system error ");
	termHex(state.status);
	termPrint(" occurred at ");
	termHexShort(state.CS);
	termPutChar(':');
	termHex(state.EIP);
	termPrint(".\n\n");
	
	termPrint("EAX: "); termHex(state.EAX); termPutChar(' ');
	termPrint("EBX: "); termHex(state.EBX); termPutChar(' ');
	termPrint("ECX: "); termHex(state.ECX); termPutChar(' ');
	termPrint("EDX: "); termHex(state.EDX); termPutChar('\n');
	
	termPrint("ESP: "); termHex(state.ESP); termPutChar(' ');
	termPrint("EBP: "); termHex(state.EBP); termPutChar('\n');
	termPrint("ESI: "); termHex(state.ESI); termPutChar(' ');
	termPrint("EDI: "); termHex(state.EDI); termPutChar('\n');
	
	termPrint("EFL: "); termHex(state.EFLAGS); termPutChar('\n');
	termPrint("ERR: "); termHex(state.error); termPutChar('\n');
	
	termPrint("\nSystem halted. Record this information and contact your system administrator.\n");
	
	asm("cli");
	asm("hlt");
	__builtin_unreachable();
}

extern void *kbdWrapper;
extern void *dfaultWrapper;

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
	
	i8259Remap(LEADER_PIC_REMAP, FOLLOWER_PIC_REMAP);
	i8259MaskAll();

	InterruptDescriptor *idt = kMalloc(sizeof(SegmentDescriptor) * 256);
	IDTRegister idtr;
	idtr.limit = sizeof(SegmentDescriptor) * 256;
	idtr.base = idt;
	
	// register double fault handler
	
	unsigned long dfcall = (unsigned long)&dfaultWrapper;
	idt[0x08].offsetLow = dfcall & 0xffff;
	idt[0x08].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	idt[0x08].mustBeZero = 0;
	idt[0x08].type = 0x8e; /* INTERRUPT_GATE */
	idt[0x08].offsetHigh = (dfcall & 0xffff0000) >> 16;
	
	// register keyboard handler
	
	unsigned long kbdcall = (unsigned long)&kbdWrapper;
	idt[0x31].offsetLow = kbdcall & 0xffff;
	idt[0x31].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	idt[0x31].mustBeZero = 0;
	idt[0x31].type = 0x8e; /* INTERRUPT_GATE */
	idt[0x31].offsetHigh = (kbdcall & 0xffff0000) >> 16;
	
	i8259ClearMask(1);
	
	__asm__ __volatile__ ("lidt (%0)": : "r" (&idtr));
	
	
	consPrint(consoles[0], "\nDumping keycodes now, ctrl to exit:\n");
	asm("sti");
	
	char spinner[] = "|/-\\";
	int spinnerIdx = 0;
	while (!done) {
		consPrint(consoles[1], "\b");
		consPutChar(consoles[1], spinner[spinnerIdx]);
		spinnerIdx = (spinnerIdx + 1) % 4;
	}
	
	consPutChar(consoles[1], '\n');
	consPrint(consoles[1], "...done!\n");
	
	consPutChar(consoles[0], '\n');
	consPrint(consoles[0], "Exiting.\n");

end:
	// should be unreachable, double fault if we ever get here
	asm("int $255");

	// for (;;) termPrint("ANIMOSITISOMIN");
}

