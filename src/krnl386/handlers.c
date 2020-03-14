#include <vga.h>
#include <syslib/console.h>
#include <conshelper.h>
#include <irq.h>
#include <8259.h>
#include <port.h>
#include <syslib/genos.h>

void kbd(intState state) {
	//consPrint(consoles[0], "Interrupted!\n");
	unsigned char code = inb(0x60);
	if (state.status == 0xDEADBEEF) {
		consHexByte(consoles[0], code);
		consPutChar(consoles[0], ' ');
	}
	i8259EOI(0);
}

void bugcheck(intState state) {
	termInitColor(vgaEntColor(vgaWhite, vgaRed));
	termPrint("\n");
	termPrint(OSVERSION);
	termPrint("\n\n");
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

