#include "stdio.h"
#include "stdint.h"
#include "gfx.h"
#include "cursor.h"
#include "windows.h"
#include "CloseGL.h"
#include "font.h"
#include "video.h"

#define BORDER_SIZE   2
#define CROSS_SIZE   14

#define TITLEBAR_H   18
#define WIN_BORDER   0xFF808080
#define WIN_TITLE_BG 0xFF1E3A8A
#define TEXT_COLOR   0xFFF8FAFC

#define MAX_WINDOWS 5

static uint8_t total_windows_created = 0;
static window_t windows[MAX_WINDOWS];

static bool is_dirty = false;

static void delete_window(uint8_t number) {
    int32_t x = windows[number].x;
    int32_t y = windows[number].y;
    int32_t width = windows[number].width;
    int32_t height = windows[number].height;

    gfx_fill(x, y, width, height, rgb(83, 109, 254));
    windows[number] = (window_t){};
    total_windows_created--;
    is_dirty = true;
}

static void draw_cross(int cx, int cy, int size, uint32_t color) {
    for (int i = 0; i < size; i++) {
        gfx_pixel(cx + i, cy + i, color);
        gfx_pixel(cx + i + 1, cy + i, color);

        gfx_pixel(cx + size - 1 - i, cy + i, color);
        gfx_pixel(cx + size - i, cy + i, color);
    }
}

bool is_windows_dirty(void) {
    bool dirty = is_dirty;
    is_dirty = false;
    return dirty;
}

void create_window(window_t* window) {
    font_set_scale(1);
    render_rectangle(window->x, window->y, window->width, window->height, WIN_BORDER);

    int title_x = window->x + BORDER_SIZE;
    int title_y = window->y + BORDER_SIZE;
    int title_w = window->width - (BORDER_SIZE * 2);
    render_rectangle(title_x, title_y, title_w, TITLEBAR_H, WIN_TITLE_BG);

    int text_col = (window->x / FONT_SIZE) + 1;
    int text_row = (window->y / FONT_SIZE) + 1;
    cursor_set(text_col, text_row);
    print("%C%s", TEXT_COLOR, window->title);

    int cross_x = title_x + title_w - CROSS_SIZE - 2;
    int cross_y = title_y + 2;

    render_rectangle(cross_x, cross_y, CROSS_SIZE, CROSS_SIZE, 0xFFC0C0C0);
    draw_cross(cross_x + 3, cross_y + 3, 7, 0xFF000000);

    int dx = window->x + BORDER_SIZE;
    int dy = window->y + BORDER_SIZE + TITLEBAR_H;
    int body_w = window->width - (BORDER_SIZE * 2);
    int body_h = window->height - (BORDER_SIZE * 2) - TITLEBAR_H;

    render_rectangle(dx, dy, body_w, body_h, window->color);

    is_dirty = true;
    windows[total_windows_created++] = *window;
}

bool windows_manager_handle_mouse(int32_t x, int32_t y, bool pressed, bool released) {
    if (total_windows_created == 0) return false;

    if (pressed) {
        for (int i = MAX_WINDOWS - 1; i >= 0; i--) {
            if (windows[i].width == 0) continue;

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
                return true;
            }
        }
    }

    return false;
}
