#include "stdint.h"
#include "zrn_syscall.h"
#include "time.h"
#include "stdio.h"

void sleep(uint32_t ms) {
    ((sys_sleep_t)SYSCALL_TABLE[1])(ms);
}

void get_time(struct time_t* t) {
    ((sys_get_time_t)SYSCALL_TABLE[2])(t);
}

void print_format_time(void) {
    struct time_t t;
    get_time(&t);
    print("%02d:%02d:%02d", t.hours, t.minutes, t.seconds);
}
