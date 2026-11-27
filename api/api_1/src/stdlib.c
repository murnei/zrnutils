#include "zrn_syscall.h"

void* kmalloc(uint32_t size) {
    return ((sys_kmalloc_t)SYSCALL_TABLE[SYS_KMALLOC])(size);
}

void kfree(void* ptr) {
    ((sys_kfree_t)SYSCALL_TABLE[SYS_KFREE])(ptr);
}

int abs(int num) {
    return (num < 0) ? -num : num;
}
