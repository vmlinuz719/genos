#include <deprecated/strlen.h>

size_t deprecatedStrlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

