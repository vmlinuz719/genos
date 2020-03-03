#include <syslib/heap.h>
#include <syslib/console.h>
#include <addm.h>
#include <port.h>
#include <serial.h>
#include <deprecated/strlen.h>

// Serial console driver

typedef struct serial {
	uint16_t port;
} SerialData;

void serialPutChar(void *data, char c) {
	outb(((SerialData *)data)->port, c);
}

size_t serialNPrint(void *data, char *str, size_t size) {
	for (size_t x = 0; x < size; x++) {
		serialPutChar(data, str[x]);
	}
	
	return size;
}

size_t serialPrint(void *data, char *str) {
	return serialNPrint(data, str, deprecatedStrlen(str));
}

void serialPageBrk(void *data) {
	serialPutChar(data, 0xC);
}

void *initSerial(void *uint16) {
	Console *cons = NULL;
	
	cons = kMalloc(sizeof(Console));
	if (cons == NULL) return NULL;
	cons->putchar = serialPutChar;
	cons->print = serialPrint;
	cons->nprint = serialNPrint;
	cons->pagebrk = serialPageBrk;
	
	cons->data = kMalloc(sizeof(SerialData));
	if (cons->data == NULL) {
		kFree(cons);
		return NULL;
	}
	
	((SerialData *)(cons->data))->port = *(uint16_t *)uint16;
	
	return cons;
}

bool destroySerial(void *console) {
	Console *cons = (Console *) console;
	if (cons == NULL || cons->putchar != serialPutChar) return false;
	
	if (cons->data != NULL) kFree(cons->data);
	kFree(cons);
	return true;
}

Driver serialDrv = {initSerial, destroySerial};

