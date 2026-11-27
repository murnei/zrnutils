#ifndef TIME_H
#define TIME_H

#include "stdint.h"

struct time_t {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
};

void sleep(uint32_t ms);
void get_time(struct time_t* t);
void print_format_time();

#endif
