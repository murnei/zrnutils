#ifndef BUFFER_H
#define BUFFER_H

#include "stdint.h"

#define BUFFER_CAPACITY 16384

typedef struct {
    uint8_t data[BUFFER_CAPACITY];
    uint32_t length;
    uint32_t cursor;
} buffer_t;

void buffer_clear(buffer_t* buffer);
bool buffer_insert(buffer_t* buffer, uint8_t ch);
bool buffer_backspace(buffer_t* buffer);
bool buffer_delete(buffer_t* buffer);

void buffer_move_left(buffer_t* buffer);
void buffer_move_right(buffer_t* buffer);
void buffer_move_up(buffer_t* buffer);
void buffer_move_down(buffer_t* buffer);
void buffer_move_home(buffer_t* buffer);
void buffer_move_end(buffer_t* buffer);

bool buffer_line_is(const buffer_t* buffer, const char* text);
void buffer_drop_line(buffer_t* buffer);

#endif
