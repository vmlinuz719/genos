#if !defined(_DEPRECATED_STRLEN_H_)
#define _DEPRECATED_STRLEN_H_

#include <stddef.h>
#include <stdint.h>

size_t deprecatedStrlen(const char* str);
void int2Hex(uint64_t x, char *buf, size_t size);

#endif

