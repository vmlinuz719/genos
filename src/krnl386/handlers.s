.globl		_kbd
.align		4

_kbd:
	pusha
	cld
	push	$0xDEADBEEF
	call	kbd
	pop		%eax
	popa
	iret
	
.globl		_doublefault
.align		4

_doublefault:
	pusha
	cld
	push	$0xFFFF0001 // !SYS-F-UNHANDLED, unhandled exception in kernel!
	call	bugcheck
	// no provision to return from here!

