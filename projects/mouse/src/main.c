#include "CloseGL.h"
#include "stdint.h"
#include "mouse.h"
#include "zrn.h"
#include "gfx.h"

#define SCREEN_W 1024
#define SCREEN_H 768

#define CURSOR_W 11
#define CURSOR_H 17

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000

static int32_t mouse_x = SCREEN_W / 2;
static int32_t mouse_y = SCREEN_H / 2;

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

static int32_t clamp(int32_t v, int32_t lo, int32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void render_cursor(int32_t start_x, int32_t start_y, const uint8_t* glyph, bool erase) {
    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            uint8_t px = glyph[y * CURSOR_W + x];
            if (px == 0) continue;

            uint32_t color = erase ? BLACK : ((px == 1) ? BLACK : WHITE);
            gfx_pixel(start_x + x, start_y + y, color);
        }
    }
}

ZRN_ENTRY
void main() {
    if (!gfx_open(SCREEN_W, SCREEN_H))
        return;

    mouse_event_t event;

    gfx_fill(0, 0, SCREEN_W, SCREEN_H, BLACK);
    render_cursor(mouse_x, mouse_y, (const uint8_t*)cursor_glyph, false);
    gfx_present();

    while (1) {
        bool moved = false;
        int32_t prev_x = mouse_x;
        int32_t prev_y = mouse_y;

        while (is_mouse_event()) {
            get_mouse_event(&event);
            mouse_x += event.dx * 2;
            mouse_y += event.dy * 2;
            moved = true;
        }

        if (moved) {
            mouse_x = clamp(mouse_x, 0, SCREEN_W - CURSOR_W);
            mouse_y = clamp(mouse_y, 0, SCREEN_H - CURSOR_H);

            if (mouse_x != prev_x || mouse_y != prev_y) {
                render_cursor(prev_x, prev_y, (const uint8_t*)cursor_glyph, true);
                render_cursor(mouse_x, mouse_y, (const uint8_t*)cursor_glyph, false);
                gfx_present();
            }
        }
    }
}

