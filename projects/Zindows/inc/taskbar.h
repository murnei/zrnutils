#ifndef TASK_BAR_H
#define TASK_BAR_H

#include "stdint.h"

#define TASKBAR_H 28

typedef struct {
    const char* path;
    void (*on_click)(void);
    bool active;
} button_t;

bool is_taskbar_dirty(void);
void taskbar_init(void);
bool taskbar_manager_handle_mouse(int32_t x, int32_t y, bool pressed);
void add_taskbar_button(button_t* button);
void taskbar_update_clocks(void);

#endif
