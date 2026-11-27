#include "stdint.h"
#include "zrn_syscall.h"
#include "font.h"
#include "cursor.h"
#include "gfx.h"

static uint32_t surface_width = 0;
static uint32_t surface_height = 0;
static uint32_t* backbuffer;
static bool opened = false;
static uint32_t dirty_top = 0;
static uint32_t dirty_bottom = 0;

static inline void local_mark_dirty(uint32_t top, uint32_t bottom) {
    if (dirty_top >= dirty_bottom) {
        dirty_top = top;
        dirty_bottom = bottom;
        return;
    }
    if (top < dirty_top) dirty_top = top;
    if (bottom > dirty_bottom) dirty_bottom = bottom;
}

void* gfx_get_backbuffer(void) {
    return ((sys_get_backbuffer_t)SYSCALL_TABLE[SYS_GET_BACKBUFFER])();
}

void gfx_mark_dirty(uint32_t top, uint32_t bottom) {
    ((sys_mark_dirty_t)SYSCALL_TABLE[SYS_MARK_DIRTY])(top, bottom);
}

bool gfx_open(uint32_t width, uint32_t height) {
    if (opened) return false;
    if (!((sys_display_open_t)SYSCALL_TABLE[SYS_DISPLAY_OPEN])(width, height)) return false;
    
    backbuffer = gfx_get_backbuffer();
    if (!backbuffer) {
        ((sys_display_close_t)SYSCALL_TABLE[SYS_DISPLAY_CLOSE])();
        return false;
    }

    surface_width = width;
    surface_height = height;
    dirty_top = 0;
    dirty_bottom = 0;
    opened = true;
    return true;
}

bool gfx_close(void) {
    bool was_open = ((sys_display_close_t)SYSCALL_TABLE[SYS_DISPLAY_CLOSE])();

    backbuffer = NULL;
    surface_width = 0;
    surface_height = 0;
    dirty_top = 0;
    dirty_bottom = 0;
    opened = false;
    return was_open;
}

bool gfx_is_open(void) {
    return opened;
}

uint32_t gfx_width(void) {
    return surface_width;
}

uint32_t gfx_height(void) {
    return surface_height;
}

void gfx_present(void) {
    if (!opened) return;
    if (dirty_top < dirty_bottom) {
        gfx_mark_dirty(dirty_top, dirty_bottom);
        dirty_top = 0;
        dirty_bottom = 0;
    }
    ((sys_display_present_t)SYSCALL_TABLE[SYS_DISPLAY_PRESENT])();
}

void gfx_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!opened || x >= surface_width || y >= surface_height) return;
    backbuffer[y * surface_width + x] = color;
    local_mark_dirty(y, y + 1);
}

uint32_t gfx_get_pixel(int32_t x, int32_t y) {
    if (!opened) return 0;
    return ((sys_display_get_pixel_t)SYSCALL_TABLE[SYS_DISPLAY_GET_PIXEL])(x, y);
}

void gfx_fill(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    if (!opened) return;
    ((sys_display_fill_t)SYSCALL_TABLE[SYS_DISPLAY_FILL])(x, y, width, height, color);
}

void gfx_clear(uint32_t color) {
    if (!opened) return;
    cursor_erase();
    gfx_fill(0, 0, surface_width, surface_height, color);
    cursor_set(0, 0);
    gfx_present();
}

void gfx_glyph(uint32_t x, uint32_t y, uint8_t ch, uint32_t color) {
    const uint8_t* glyph = font_glyph(ch);
    if (!glyph) return;

    uint32_t scale = font_scale();

    for (uint32_t row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        uint32_t col = 0;

        while (col < 8) {
            if (!(bits & (0x80 >> col))) {
                col++;
                continue;
            }

            uint32_t run = 1;
            while (col + run < 8 && (bits & (0x80 >> (col + run)))) run++;

            gfx_fill(x + col * scale, y + row * scale, run * scale, scale, color);
            col += run;
        }
    }
}

void gfx_putc(uint8_t ch, uint32_t color) {
    if (!opened) return;

    uint32_t cell = font_cell();

    cursor_erase();

    if (ch == '\n') {
        cursor_newline();
        return;
    }

    if (ch == '\t') {
        cursor_move(4, 0);
        return;
    }

    if (cursor_col() < CURSOR_MARGIN) {
        cursor_set(CURSOR_MARGIN, cursor_row());
    }

    if ((uint32_t)cursor_pixel_x() + cell > surface_width) {
        cursor_set(CURSOR_MARGIN, cursor_row() + 1);
    }

    if ((uint32_t)cursor_pixel_y() + cell > surface_height) {
        cursor_set(CURSOR_MARGIN, 0);
    }

    gfx_glyph((uint32_t)cursor_pixel_x(), (uint32_t)cursor_pixel_y(), ch, color);
    cursor_move(1, 0);
}
