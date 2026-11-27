#include "stdint.h"
#include "ps2_mouse.h"
#include "mouse.h"
#include "gfx.h"
#include "workspace.h"

#define SCREEN_W 1024
#define SCREEN_H 768

#define CURSOR_W 11
#define CURSOR_H 17

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000

static int32_t mouse_x = SCREEN_W / 2;
static int32_t mouse_y = SCREEN_H / 2;

static bool is_dirty = false;
static uint8_t prev_buttons;

static const uint8_t cursor_glyph[CURSOR_H][CURSOR_W] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
    {1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1},
    {1, 2, 2, 1, 2, 2, 1, 0, 0, 0, 0},
    {1, 2, 1, 0, 1, 2, 2, 1, 0, 0, 0},
    {1, 1, 0, 0, 1, 2, 2, 1, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}
};

static uint32_t cursor_back[CURSOR_H][CURSOR_W];

static int32_t clamp(int32_t v, int32_t lo, int32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void save_cursor_background(int32_t start_x, int32_t start_y, uint32_t (*back)[CURSOR_W]) {
    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            back[y][x] = gfx_get_pixel(start_x + x, start_y + y);
        }
    }
}

static void erase_cursor(int32_t start_x, int32_t start_y, const uint32_t (*back)[CURSOR_W]) {
    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            gfx_pixel(start_x + x, start_y + y, back[y][x]);
        }
    }
    is_dirty = true;
}

static void render_cursor(int32_t start_x, int32_t start_y, const uint8_t (*glyph)[CURSOR_W]) {
    save_cursor_background(start_x, start_y, cursor_back);

    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            uint8_t px = glyph[y][x];
            if (px == 0) continue;

            gfx_pixel(start_x + x, start_y + y,
                      px == 1 ? BLACK : WHITE);
        }
    }
    is_dirty = true;
}

void mouse_update(void) {
    mouse_event_t event;
    int32_t prev_x = mouse_x;
    int32_t prev_y = mouse_y;

    while (is_mouse_event()) {
        get_mouse_event(&event);
        int32_t old_x = mouse_x;
        int32_t old_y = mouse_y;

        mouse_x = clamp(mouse_x + event.dx * 2, 0, SCREEN_W - CURSOR_W);
        mouse_y = clamp(mouse_y + event.dy * 2, 0, SCREEN_H - CURSOR_H);

        if (mouse_x != old_x || mouse_y != old_y) {
            erase_cursor(old_x, old_y, cursor_back);
            render_cursor(mouse_x, mouse_y, cursor_glyph);
        }

        bool left_pressed = (event.button & 1) && !(prev_buttons & 1);
        bool left_released = !(event.button & 1) && (prev_buttons & 1);
        prev_buttons = event.button;

        workspace_manager(mouse_x, mouse_y, left_pressed, left_released);
    }
}

void mouse_init(void) {
    render_cursor(mouse_x, mouse_y, cursor_glyph);

}

void mouse_erase(void) {
    erase_cursor(mouse_x, mouse_y, cursor_back);
}

void mouse_render(void) {
    render_cursor(mouse_x, mouse_y, cursor_glyph);
}

bool mouse_is_dirty(void) {
    bool dirty = is_dirty;
    is_dirty = false;
    return dirty;
}
