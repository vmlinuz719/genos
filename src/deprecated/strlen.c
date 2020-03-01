#include <deprecated/strlen.h>

size_t deprecatedStrlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

void int2Hex(uint64_t x, char *buf, size_t size) {
	// expect buf to be n characters long - last one gets \0
	buf[size - 1] = '\0';

	uint64_t num = x;
	int i = size - 1;

	// (void)x;
	do {
		buf[--i] = "0123456789ABCDEF"[num % 16];
		num >>= 4;
	} while (num > 0 && i);

	while (--i >= 0) buf[i] = '0';
}
