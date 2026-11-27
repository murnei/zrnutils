#include "stdint.h"
#include "stdio.h"
#include "cursor.h"
#include "font.h"
#include "system.h"
#include "time.h"
#include "video.h"
#include "images.h"
#include "mouse.h"
#include "config.h"
#include "windows.h"
#include "taskbar.h"
#include "CloseGL.h"

#define MAX_TASKBAR_BUTTONS 33
#define CLOCK_INTERVAL 50
#define BTN_SIZE 24
#define BTN_GAP 4

static button_t taskbar_buttons[MAX_TASKBAR_BUTTONS];
static int timer_ticks = 0;
static bool is_dirty = false;

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

static bool handle_taskbar_click(int32_t mouse_x, int32_t mouse_y) {
    if (mouse_y < SCREEN_HEIGHT - TASKBAR_H) {
        return false;
    }

    int32_t cy = (SCREEN_HEIGHT - TASKBAR_H) + (TASKBAR_H - BTN_SIZE) / 2;

    if (mouse_y < cy || mouse_y >= cy + BTN_SIZE) {
        return false;
    }

    for (int i = 0; i < MAX_TASKBAR_BUTTONS; i++) {
        if (!taskbar_buttons[i].active) continue;

        int32_t cx = BTN_GAP + i * (BTN_SIZE + BTN_GAP);

        if (mouse_x >= cx && mouse_x < cx + BTN_SIZE) {
            if (taskbar_buttons[i].on_click != NULL) {
                taskbar_buttons[i].on_click();
            }
            return true;
        }
    }

    return false;
}

static void update_clock(void) {
    static int last_minute = -1;
    struct time_t time;
    get_time(&time);

    if (time.minutes == last_minute) return;
    last_minute = time.minutes;

    font_set_scale(1);
    cursor_set(122, 94);
    mouse_erase();
    render_rectangle_fill(122 * 8, 94 * 8, 5 * 8, 8, rgb(192, 192, 192));
    print("%C%02d:%02d", rgb(0, 0, 0), time.hours, time.minutes);
    mouse_render();

    is_dirty = true;
}

bool is_taskbar_dirty(void) {
    bool dirty = is_dirty;
    is_dirty = false;
    return dirty;
}

void taskbar_update_clocks(void) {
    if (timer_ticks + 1 == CLOCK_INTERVAL) {
        timer_ticks = 0;
        update_clock();
        return;
    }
    timer_ticks++;
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

static void shutdown_on_click(void) { shutdown(); }

void taskbar_init(void) {
    render_rectangle_fill(0, SCREEN_HEIGHT - TASKBAR_H, SCREEN_WIDTH, TASKBAR_H, rgb(192, 192, 192));
    update_clock();

    button_t shutdown_btn = {
        .path = "/home/pics/shut.bmp",
        .on_click = shutdown_on_click
    };

    add_taskbar_button(&shutdown_btn);
}

bool taskbar_manager_handle_mouse(int32_t x, int32_t y, bool pressed) {
    if (pressed) {
        return handle_taskbar_click(x, y);
    }
    return false;
}
