#if !defined(_PORT_H_)
#define _PORT_H_
#include <stddef.h>
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
            asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile ( "inb %1, %0"
                        : "=a"(ret)
                        : "Nd"(port) );
        return ret;
}

static inline void ioWait(void) {
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

#endif
