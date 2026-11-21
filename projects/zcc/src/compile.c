#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "zrn_syscall.h"
#include "compile.h"
#include "codegen.h"
#include "error.h"

#define RET 0xC3

#define BUF_END 0x80

bool is_return = false;

struct api_func {
    const char* name;
    uint8_t syscall_id;
    uint8_t arg_count;
};

static const uint8_t code_clear[] = {
    0xA1, 0x00,
    0x00, 0x05,
    0x00, 0x8B,
    0x00, 0xFF,
    0xD0
};

static const struct api_func api[] = {
    { "clear", SYS_CLEAR, 0 },
    { "sleep", SYS_SLEEP, 1 },
    { "putc", SYS_PUTC, 2 },
    { "exit", SYS_EXIT, 0 }
};

const struct api_func* find_api(const char* name) {
    for (uint32_t i = 0; i < sizeof(api) / sizeof(api[0]); i++) {
        if (strcmp(name, api[i].name) == 0) return &api[i];
    }

    return 0;
}

void add_header(struct buffer_t* output) {
    emit8(0x2E, output);
    emit8(0x5A, output);
    emit8(0x52, output);
    emit8(0x4E, output);
}

void compile(const char* line, struct buffer_t* output) {
    while (*line == ' ' || *line == '\t' || *line == '\r' || *line == '\n') {
        line++;
    }
    if (*line == '\0') return;

    if (strcmp(line, "clear()") == 0) {
        emit_bytes(code_clear, sizeof(code_clear), output);
    } else if (strcmp(line, "return") == 0) {
        emit8(RET, output);
        is_return = true;
    }
}

int compiler_main(const char* name, struct buffer_t* buf, struct buffer_t* output) {
    uint32_t start = 0;
    add_header(output);
    for (uint32_t i = 0; i <= buf->length; i++) {
        uint8_t ch = buf->data[i];
        if (ch == BUF_END || ch == '\0') {
            buf->data[i] = '\0';
            compile((const char*)&buf->data[start], output);
            break;
        }
        if (ch == ';' || ch == '\n') {
            buf->data[i] = '\0';
            compile((const char*)&buf->data[start], output);
            start = i + 1;
        }
    }

    if (!is_return) {
        emit8(RET, output);
        return HAS_NOT_RETURN;
    }

    return NO_ERROR;
}
