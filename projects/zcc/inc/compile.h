#ifndef COMPILE_H
#define COMPILE_H

#define BUF_CAPACITY 4096

struct buffer_t {
    uint8_t data[BUF_CAPACITY];
    uint32_t length;
};

int compiler_main(const char* name, struct buffer_t* buf, struct buffer_t* output);

#endif
