#ifndef TASK_BAR_H
#define TASK_BAR_H

#include "stdint.h"

#define TASKBAR_H 28

bool is_taskbar_dirty(void);
void taskbar_init(void);
bool taskbar_manager_handle_mouse(int32_t x, int32_t y, bool pressed);

#endif
