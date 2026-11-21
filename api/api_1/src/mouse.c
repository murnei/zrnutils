#include "mouse.h"
#include "zrn_syscall.h"

bool is_mouse_event(void) {
    return ((sys_is_mouse_event_t)SYSCALL_TABLE[SYS_IS_MOUSE_EVENT])();
}

void get_mouse_event(mouse_event_t* event) {
    ((sys_get_mouse_event_t)SYSCALL_TABLE[SYS_GET_MOUSE_EVENT])(event);
}
