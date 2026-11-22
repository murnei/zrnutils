#include "stdio.h"
#include "stdint.h"
#include "gfx.h"
#include "cursor.h"
#include "windows.h"
#include "CloseGL.h"
#include "font.h"
#include "video.h"
#include "mouse.h"
#include "taskbar.h"

#define SCREEN_W      1024
#define SCREEN_H      768 - TASKBAR_H

#define BORDER_SIZE   2
#define CROSS_SIZE    14

#define TITLEBAR_H    18
#define WIN_BORDER    0xFF808080
#define WIN_TITLE_BG  0xFF1E3A8A
#define TEXT_COLOR    0xFFF8FAFC
#define DESKTOP_BG    rgb(83, 109, 254)

#define MAX_WINDOWS   5

static uint8_t total_windows_created = 0;
static window_t windows[MAX_WINDOWS];

static int8_t dragging_idx = -1;
static int32_t drag_offset_x = 0;
static int32_t drag_offset_y = 0;
static int32_t drag_preview_x = 0;
static int32_t drag_preview_y = 0;

static bool is_dirty = false;

static void bring_window_to_front(uint8_t idx) {
    if (idx >= total_windows_created - 1) return;

    window_t active = windows[idx];
    for (uint8_t i = idx; i < total_windows_created - 1; i++) {
        windows[i] = windows[i + 1];
    }
    windows[total_windows_created - 1] = active;
}

static bool rects_intersect(int32_t x1, int32_t y1, int32_t w1, int32_t h1,
                            int32_t x2, int32_t y2, int32_t w2, int32_t h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 &&
            y1 < y2 + h2 && y1 + h1 > y2);
}

static int32_t clamp(int32_t v, int32_t lo, int32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void draw_cross(int cx, int cy, int size, uint32_t color) {
    for (int i = 0; i < size; i++) {
        gfx_pixel(cx + i, cy + i, color);
        gfx_pixel(cx + i + 1, cy + i, color);

        gfx_pixel(cx + size - 1 - i, cy + i, color);
        gfx_pixel(cx + size - i, cy + i, color);
    }
}

static void draw_window(window_t* window) {
    mouse_erase();
    font_set_scale(1);
    render_rectangle_fill(window->x, window->y, window->width, window->height, WIN_BORDER);

    int title_x = window->x + BORDER_SIZE;
    int title_y = window->y + BORDER_SIZE;
    int title_w = window->width - (BORDER_SIZE * 2);
    render_rectangle_fill(title_x, title_y, title_w, TITLEBAR_H, WIN_TITLE_BG);

    int text_col = (window->x / FONT_SIZE) + 1;
    int text_row = (window->y / FONT_SIZE) + 1;
    cursor_set(text_col, text_row);
    print("%C%s", TEXT_COLOR, window->title);

    int cross_x = title_x + title_w - CROSS_SIZE - 2;
    int cross_y = title_y + 2;

    render_rectangle_fill(cross_x, cross_y, CROSS_SIZE, CROSS_SIZE, 0xFFC0C0C0);
    draw_cross(cross_x + 3, cross_y + 3, 7, 0xFF000000);

    int dx = window->x + BORDER_SIZE;
    int dy = window->y + BORDER_SIZE + TITLEBAR_H;
    int body_w = window->width - (BORDER_SIZE * 2);
    int body_h = window->height - (BORDER_SIZE * 2) - TITLEBAR_H;

    render_rectangle_fill(dx, dy, body_w, body_h, window->color);
    mouse_render();
    is_dirty = true;
}

static void repair_area(const window_t* bad_window) {
    mouse_erase();
    gfx_fill(bad_window->x, bad_window->y,
             bad_window->width, bad_window->height, DESKTOP_BG);
    mouse_render();

    for (uint8_t i = 0; i < total_windows_created; i++) {
        if (rects_intersect(bad_window->x, bad_window->y, bad_window->width, bad_window->height,
                            windows[i].x, windows[i].y, windows[i].width, windows[i].height)) {
            draw_window(&windows[i]);
        }
    }
}

static void delete_window(uint8_t number) {
    window_t old_win = windows[number];

    for (uint8_t i = number; i < total_windows_created - 1; i++) {
        windows[i] = windows[i + 1];
    }
    total_windows_created--;
    windows[total_windows_created] = (window_t){};

    repair_area(&old_win);
    is_dirty = true;
}

static void on_drag(uint8_t idx, int32_t x, int32_t y) {
    int32_t new_x = clamp(x - drag_offset_x, 0, SCREEN_W - windows[idx].width);
    int32_t new_y = clamp(y - drag_offset_y, 0, SCREEN_H - windows[idx].height);

    if (new_x != drag_preview_x || new_y != drag_preview_y) {
        mouse_erase();
        render_rectangle_xor(drag_preview_x, drag_preview_y,
                             windows[idx].width, windows[idx].height);
        drag_preview_x = new_x;
        drag_preview_y = new_y;
        render_rectangle_xor(drag_preview_x, drag_preview_y,
                             windows[idx].width, windows[idx].height);
        mouse_render();
        is_dirty = true;
    }
}

static void on_release(uint8_t idx) {
    mouse_erase();
    render_rectangle_xor(drag_preview_x, drag_preview_y,
                         windows[idx].width, windows[idx].height);
    mouse_render();

    window_t old_win = windows[idx];

    windows[idx].x = drag_preview_x;
    windows[idx].y = drag_preview_y;

    repair_area(&old_win);
    draw_window(&windows[idx]);
}

static bool on_press(int32_t x, int32_t y) {
    for (int i = total_windows_created - 1; i >= 0; i--) {
        int title_x = windows[i].x + BORDER_SIZE;
        int title_y = windows[i].y + BORDER_SIZE;
        int title_w = windows[i].width - (BORDER_SIZE * 2);
        int title_h = TITLEBAR_H;

        int cross_x = title_x + title_w - CROSS_SIZE - 2;
        int cross_y = title_y + 2;

        if (x >= cross_x && x < cross_x + CROSS_SIZE &&
            y >= cross_y && y < cross_y + CROSS_SIZE) {
            delete_window(i);
            return true;
        }

        if (x >= title_x && x < title_x + title_w &&
            y >= title_y && y < title_y + title_h) {
            if (i < total_windows_created - 1) {
                bring_window_to_front(i);
                draw_window(&windows[total_windows_created - 1]);
            }
            uint8_t top_idx = total_windows_created - 1;

            dragging_idx = top_idx;
            drag_offset_x = x - windows[top_idx].x;
            drag_offset_y = y - windows[top_idx].y;
            drag_preview_x = windows[top_idx].x;
            drag_preview_y = windows[top_idx].y;

            mouse_erase();
            render_rectangle_xor(drag_preview_x, drag_preview_y,
                                 windows[top_idx].width, windows[top_idx].height);
            mouse_render();
            is_dirty = true;
            return true;
        }

        if (x >= windows[i].x && x < windows[i].x + windows[i].width &&
            y >= windows[i].y && y < windows[i].y + windows[i].height) {
            if (i < total_windows_created - 1) {
                bring_window_to_front(i);
                draw_window(&windows[total_windows_created - 1]);
            }
            return true;
        }
    }
    return false;
}

bool is_windows_dirty(void) {
    bool dirty = is_dirty;
    is_dirty = false;
    return dirty;
}

void create_window(window_t* window) {
    draw_window(window);
    windows[total_windows_created++] = *window;
}

bool windows_manager_handle_mouse(int32_t x, int32_t y, bool pressed, bool released) {
    if (total_windows_created == 0) return false;

    if (pressed) {
        return on_press(x, y);
    }

    if (dragging_idx != -1) {
        if (released) {
            on_release(dragging_idx);
            dragging_idx = -1;
        } else {
            on_drag(dragging_idx, x, y);
        }
        return true;
    }

    return false;
}
