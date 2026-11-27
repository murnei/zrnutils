#include "stdint.h"
#include "buffer.h"

static uint32_t line_start_from(const buffer_t* buffer, uint32_t pos) {
    while (pos > 0 && buffer->data[pos - 1] != '\n') pos--;
    return pos;
}

static uint32_t line_end_from(const buffer_t* buffer, uint32_t pos) {
    while (pos < buffer->length && buffer->data[pos] != '\n') pos++;
    return pos;
}

void buffer_clear(buffer_t* buffer) {
    buffer->length = 0;
    buffer->cursor = 0;
}

bool buffer_insert(buffer_t* buffer, uint8_t ch) {
    if (buffer->length >= BUFFER_CAPACITY) return false;

    for (uint32_t i = buffer->length; i > buffer->cursor; i--) {
        buffer->data[i] = buffer->data[i - 1];
    }

    buffer->data[buffer->cursor++] = ch;
    buffer->length++;
    return true;
}

bool buffer_backspace(buffer_t* buffer) {
    if (buffer->cursor == 0) return false;

    for (uint32_t i = buffer->cursor - 1; i + 1 < buffer->length; i++) {
        buffer->data[i] = buffer->data[i + 1];
    }

    buffer->cursor--;
    buffer->length--;
    return true;
}

bool buffer_delete(buffer_t* buffer) {
    if (buffer->cursor >= buffer->length) return false;

    for (uint32_t i = buffer->cursor; i + 1 < buffer->length; i++) {
        buffer->data[i] = buffer->data[i + 1];
    }

    buffer->length--;
    return true;
}

void buffer_move_left(buffer_t* buffer) {
    if (buffer->cursor > 0) buffer->cursor--;
}

void buffer_move_right(buffer_t* buffer) {
    if (buffer->cursor < buffer->length) buffer->cursor++;
}

void buffer_move_home(buffer_t* buffer) {
    buffer->cursor = line_start_from(buffer, buffer->cursor);
}

void buffer_move_end(buffer_t* buffer) {
    buffer->cursor = line_end_from(buffer, buffer->cursor);
}

void buffer_move_up(buffer_t* buffer) {
    uint32_t cur_start = line_start_from(buffer, buffer->cursor);
    if (cur_start == 0) {
        buffer->cursor = 0;
        return;
    }

    uint32_t col = buffer->cursor - cur_start;
    uint32_t prev_end = cur_start - 1;
    uint32_t prev_start = line_start_from(buffer, prev_end);
    uint32_t prev_len = prev_end - prev_start;

    buffer->cursor = prev_start + (col < prev_len ? col : prev_len);
}

void buffer_move_down(buffer_t* buffer) {
    uint32_t cur_start = line_start_from(buffer, buffer->cursor);
    uint32_t cur_end = line_end_from(buffer, buffer->cursor);
    if (cur_end >= buffer->length) {
        buffer->cursor = buffer->length;
        return;
    }

    uint32_t col = buffer->cursor - cur_start;
    uint32_t next_start = cur_end + 1;
    uint32_t next_end = line_end_from(buffer, next_start);
    uint32_t next_len = next_end - next_start;

    buffer->cursor = next_start + (col < next_len ? col : next_len);
}

bool buffer_line_is(const buffer_t* buffer, const char* text) {
    uint32_t start = line_start_from(buffer, buffer->cursor);
    uint32_t end = line_end_from(buffer, buffer->cursor);
    uint32_t index = 0;

    while (text[index]) {
        if (start + index >= end) return false;
        if (buffer->data[start + index] != (uint8_t)text[index]) return false;
        index++;
    }

    return start + index == end;
}

void buffer_drop_line(buffer_t* buffer) {
    uint32_t start = line_start_from(buffer, buffer->cursor);
    uint32_t end = line_end_from(buffer, buffer->cursor);
    uint32_t remove_count = end - start;

    for (uint32_t i = start; i + remove_count < buffer->length; i++) {
        buffer->data[i] = buffer->data[i + remove_count];
    }

    buffer->length -= remove_count;
    buffer->cursor = start;
}
