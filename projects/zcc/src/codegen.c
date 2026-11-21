#include "stdint.h"
#include "compile.h"

void emit8(uint8_t byte, struct buffer_t *buf) {
    if (buf->length < BUF_CAPACITY) {
        buf->data[buf->length++] = byte;
    }
}

void emit_bytes(const uint8_t* data, uint32_t size, struct buffer_t* buf) {
    for (uint32_t i = 0; i < size; i++) {
        emit8(data[i], buf);
    }
}

