#ifndef WINDOWS_H
#define WINDOWS_H

#include "stdint.h"

typedef struct {
    const char* title;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    uint32_t color;
} window_t;

bool is_windows_dirty(void);
void create_window(window_t* window);
bool windows_manager_handle_mouse(int32_t x, int32_t y, bool pressed, bool realised);

#endif
