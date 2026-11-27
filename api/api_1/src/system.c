#include "zrn_syscall.h"

void reboot(void) {
    ((sys_reboot_t)SYSCALL_TABLE[SYS_REBOOT])();
}

void shutdown(void) {
    ((sys_shutdown_t)SYSCALL_TABLE[SYS_SHUTDOWN])();
}
