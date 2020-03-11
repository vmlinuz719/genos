/* Multiboot header constants */
.set ALIGN,	1<<0
.set MEMINFO,	1<<1
.set FLAGS,	ALIGN | MEMINFO
.set MAGIC,	0x1BADB002
.set CHECKSUM,	-(MAGIC + FLAGS)
.set HEAPSIZE,	16384

/* Multiboot header */
.section 	.multiboot
.align		4
.long		MAGIC
.long		FLAGS
.long		CHECKSUM

/* SysV aligned early kernel stack */
.section	bss

.align		16
stack_bottom:
	.skip	16384

stack_top:

/* GenOS KRNL386.SYS entry point! */
.section	.text
.global		_start
.type		_start,		@function

_start:
	/* Initialize stack */
	mov	$stack_top,	%esp

	/* TODO: Load paging, GDT, IDT, call C++ global constructors */

	push	%eax
	push	%ebx
	call	kmain

	/* Kernel should never return!!! */
	/* TODO: fail an assertion here */
	cli
1:	hlt
	jmp	1b

.global		_flush_gdt
.type		_flush_gdt,	@function

_flush_gdt:
	ljmp	$0x08,		$reload_CS
reload_CS:
	mov	$0x10,		%ax
	mov	%ax,		%ds
	mov     %ax,            %es
	mov     %ax,            %fs
	mov     %ax,            %gs
	mov     %ax,            %ss
	ret

/* ISR wrappers. TODO: move to new file */

.globl		kbdWrapper
.align		4

kbdWrapper:
	pusha
	cld
	push	$0xDEADBEEF
	call	kbd
	pop		%eax
	popa
	iret
	
.globl		dfaultWrapper
.align		4

dfaultWrapper:
	pusha
	cld
	push	$0x00FF0001 // !SYS-F-UNHANDLED, unhandled exception in kernel!
	call	bugcheck
	// no provision to return from here!

