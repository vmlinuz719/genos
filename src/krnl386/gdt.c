#include <gdt.h>
#include <syslib/heap.h>

GDTRegister initGDT(SegmentDescriptor **gdt) {
	GDTRegister gdtr;
	*gdt = kMalloc(sizeof(SegmentDescriptor) * 8);
	if (*gdt == NULL) {
		return (GDTRegister){0, 0};
	}
	// intStatus((int)gdt);
	// termPrint("Global Descriptor Table address\n");

	(*gdt)[0] = createDescriptor(0, 0, 0);
	(*gdt)[1] = createDescriptor(0, 0xFFFFF, (GDT_CODE_PL0));
	(*gdt)[2] = createDescriptor(0, 0xFFFFF, (GDT_DATA_PL0));
	(*gdt)[3] = createDescriptor(0, 0xFFFFF, (GDT_CODE_PL3));
	(*gdt)[4] = createDescriptor(0, 0xFFFFF, (GDT_DATA_PL3));

	gdtr.base = *gdt;
	gdtr.size = 32 * 8 - 1;

	__asm__ __volatile__ ("lgdt (%0)": : "r" (&gdtr));
	_flush_gdt();
	
	return gdtr;
}

