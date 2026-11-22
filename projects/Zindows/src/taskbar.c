#include "stdint.h"
#include "taskbar.h"
#include "video.h"
#include "config.h"
#include "images.h"
#include "mouse.h"
#include "CloseGL.h"

#define MAX_TASKBAR_BUTTONS 36
#define BTN_SIZE 24
#define BTN_GAP 4

typedef struct {
    const char* path;
    void (*on_click)(void);
    bool active;
} button_t;

static button_t taskbar_buttons[MAX_TASKBAR_BUTTONS];
static bool is_dirty;

static void draw_button(uint8_t idx) {
    int32_t cx = BTN_GAP + idx * (BTN_SIZE + BTN_GAP);
    int32_t cy = (SCREEN_HEIGHT - TASKBAR_H) + (TASKBAR_H - BTN_SIZE) / 2;

    mouse_erase();
    if (taskbar_buttons[idx].path != NULL) {
        render_bmp(taskbar_buttons[idx].path, cx, cy);
    } else {
        render_rectangle_fill(cx, cy, BTN_SIZE, BTN_SIZE, rgb(220, 38, 38));
    }
    mouse_render();
}

bool is_taskbar_dirty(void) {
    return is_dirty;
}

void add_taskbar_button(button_t* button) {
    for (uint8_t i = 0; i < MAX_TASKBAR_BUTTONS; i++) {
        if (!taskbar_buttons[i].active) {
            taskbar_buttons[i] = *button;
            taskbar_buttons[i].active = true;
            draw_button(i);
            is_dirty = true;
            return;
        }
    }
}

void taskbar_init(void) {
    render_rectangle_fill(0, SCREEN_HEIGHT - TASKBAR_H, SCREEN_WIDTH, TASKBAR_H, rgb(192, 192, 192));
}

bool taskbar_manager_handle_mouse(int32_t x, int32_t y, bool pressed) {
    return false;
}
