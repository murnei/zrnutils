#ifndef CODEGEN_H
#define CODEGEN_H

#include "compile.h"

void emit8(uint8_t byte, struct buffer_t *buf);
void emit_bytes(const uint8_t* data, uint32_t size, struct buffer_t *buf);

#endif
