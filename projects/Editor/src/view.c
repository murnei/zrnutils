#include "stdint.h"
#include "gfx.h"
#include "font.h"
#include "buffer.h"
#include "view.h"

#define BG_COLOR        gfx_rgb(24, 26, 32)
#define BAR_BG_COLOR    gfx_rgb(36, 40, 50)
#define GUTTER_BG_COLOR gfx_rgb(20, 22, 27)
#define TEXT_COLOR      gfx_rgb(225, 228, 235)
#define LINE_NUM_COLOR  gfx_rgb(95, 105, 125)
#define ACCENT_COLOR    gfx_rgb(97, 175, 239)
#define SUCCESS_COLOR   gfx_rgb(152, 195, 121)
#define MUTED_COLOR     gfx_rgb(140, 148, 165)
#define CARET_COLOR     gfx_rgb(255, 200, 87)
#define BORDER_COLOR    gfx_rgb(55, 60, 74)

#define GUTTER_COLS     5
#define MARGIN_RIGHT    1
#define RESERVED_ROWS   2

#define HINT_TEXT ":w Save   :wq Save+Quit   ESC / :q Quit"

static uint32_t scroll_row = 0;

typedef struct {
    uint32_t col;
    uint32_t row;
} layout_t;

static uint32_t total_screen_cols(void) {
    return gfx_width() / font_cell();
}

static uint32_t text_cols(void) {
    uint32_t total = total_screen_cols();
    return total > (GUTTER_COLS + MARGIN_RIGHT) ? total - GUTTER_COLS - MARGIN_RIGHT : 1;
}

static uint32_t visible_rows(void) {
    uint32_t total = gfx_height() / font_cell();
    return total > RESERVED_ROWS ? total - RESERVED_ROWS : 1;
}

static uint32_t last_row(void) {
    return gfx_height() / font_cell() - 1;
}

static uint32_t room(uint32_t total, uint32_t used) {
    return total > used ? total - used : 0;
}

static uint32_t text_length(const char* text) {
    uint32_t length = 0;
    while (text[length]) length++;
    return length;
}

static void draw_char_raw(uint32_t col, uint32_t row, uint8_t ch, uint32_t color) {
    uint32_t cell = font_cell();
    gfx_glyph(col * cell, row * cell, ch, color);
}

static void draw_string_raw(uint32_t col, uint32_t row, const char* text, uint32_t limit, uint32_t color) {
    for (uint32_t i = 0; text[i] && i < limit; i++) {
        draw_char_raw(col + i, row, (uint8_t)text[i], color);
    }
}

static void draw_text_char(uint32_t col, uint32_t row, uint8_t ch, uint32_t color) {
    draw_char_raw(col + GUTTER_COLS, row, ch, color);
}

static void draw_caret(uint32_t col, uint32_t row) {
    uint32_t cell = font_cell();
    uint32_t x = (col + GUTTER_COLS) * cell;
    uint32_t y = row * cell;
    gfx_fill(x, y, 2, cell, CARET_COLOR);
}

static void draw_line_number(uint32_t screen_row, uint32_t line_num) {
    char buf[5];
    buf[0] = (char)('0' + ((line_num / 100) % 10));
    buf[1] = (char)('0' + ((line_num / 10) % 10));
    buf[2] = (char)('0' + (line_num % 10));
    buf[3] = ' ';
    buf[4] = '\0';

    if (line_num < 100) buf[0] = ' ';
    if (line_num < 10)  buf[1] = ' ';

    draw_string_raw(0, screen_row, buf, 4, LINE_NUM_COLOR);
}

static void draw_uint(uint32_t col, uint32_t row, uint32_t value, uint32_t color) {
    char buf[12];
    uint32_t idx = 0;

    if (value == 0) {
        buf[idx++] = '0';
    } else {
        char tmp[12];
        uint32_t t = 0;
        while (value > 0) {
            tmp[t++] = (char)('0' + (value % 10));
            value /= 10;
        }
        while (t > 0) {
            buf[idx++] = tmp[--t];
        }
    }
    buf[idx] = '\0';
    draw_string_raw(col, row, buf, idx, color);
}

static bool layout_next(layout_t* layout, uint8_t ch, uint32_t cols) {
    if (ch == '\n') {
        layout->row++;
        layout->col = 0;
        return false;
    }

    if (layout->col >= cols) {
        layout->row++;
        layout->col = 0;
    }

    return true;
}

static void layout_settle(layout_t* layout, uint32_t cols) {
    if (layout->col >= cols) {
        layout->row++;
        layout->col = 0;
    }
}

void view_prompt(const char* prompt, const char* input) {
    uint32_t cell = font_cell();
    uint32_t total_cols = total_screen_cols();
    uint32_t prompt_len = text_length(prompt);
    uint32_t input_len = text_length(input);

    gfx_fill(0, 0, gfx_width(), gfx_height(), BG_COLOR);
    gfx_fill(0, 0, gfx_width(), cell, BAR_BG_COLOR);
    draw_string_raw(1, 0, "ZAREBOON EDITOR", total_cols, ACCENT_COLOR);

    uint32_t box_y = 3 * cell;
    gfx_fill(cell, box_y - cell / 2, gfx_width() - 2 * cell, cell * 2, BAR_BG_COLOR);

    draw_string_raw(2, 3, prompt, total_cols, ACCENT_COLOR);
    draw_string_raw(2 + prompt_len, 3, input, room(total_cols, 3 + prompt_len), TEXT_COLOR);

    uint32_t caret_x = (2 + prompt_len + input_len) * cell;
    gfx_fill(caret_x, 3 * cell, 2, cell, CARET_COLOR);

    gfx_present();
}

void view_editor(const buffer_t* buffer, const char* name, const char* status) {
    uint32_t cell = font_cell();
    uint32_t scr_cols = total_screen_cols();
    uint32_t cols = text_cols();
    uint32_t rows = visible_rows();

    layout_t layout = { 0, 0 };
    layout_t caret = { 0, 0 };

    for (uint32_t i = 0; i <= buffer->length; i++) {
        if (i == buffer->cursor) {
            layout_settle(&layout, cols);
            caret = layout;
        }
        if (i == buffer->length) break;

        if (layout_next(&layout, buffer->data[i], cols)) {
            layout.col++;
        }
    }
    layout_settle(&layout, cols);
    uint32_t total = layout.row + 1;

    if (caret.row < scroll_row) {
        scroll_row = caret.row;
    } else if (caret.row >= scroll_row + rows) {
        scroll_row = caret.row - rows + 1;
    }

    gfx_fill(0, 0, gfx_width(), gfx_height(), BG_COLOR);

    uint32_t gutter_px = (GUTTER_COLS - 1) * cell;
    gfx_fill(0, cell, gutter_px, rows * cell, GUTTER_BG_COLOR);
    gfx_fill(gutter_px, cell, 1, rows * cell, BORDER_COLOR);

    gfx_fill(0, 0, gfx_width(), cell, BAR_BG_COLOR);
    draw_string_raw(1, 0, "EDIT", scr_cols, ACCENT_COLOR);
    draw_string_raw(6, 0, name, room(scr_cols, 22), TEXT_COLOR);

    if (scr_cols > 20) {
        draw_string_raw(scr_cols - 15, 0, "Bytes:", 6, MUTED_COLOR);
        draw_uint(scr_cols - 8, 0, buffer->length, TEXT_COLOR);
    }

    uint32_t visible_count = total > scroll_row ? total - scroll_row : 0;
    if (visible_count > rows) visible_count = rows;
    for (uint32_t r = 0; r < visible_count; r++) {
        draw_line_number(r + 1, scroll_row + r + 1);
    }

    layout.col = 0;
    layout.row = 0;
    for (uint32_t i = 0; i < buffer->length; i++) {
        uint8_t ch = buffer->data[i];
        if (!layout_next(&layout, ch, cols)) continue;

        if (layout.row >= scroll_row && layout.row < scroll_row + rows) {
            draw_text_char(layout.col, layout.row - scroll_row + 1, ch, TEXT_COLOR);
        }
        layout.col++;
    }

    if (caret.row >= scroll_row && caret.row < scroll_row + rows) {
        draw_caret(caret.col, caret.row - scroll_row + 1);
    }

    uint32_t footer_row = last_row();
    gfx_fill(0, footer_row * cell, gfx_width(), cell, BAR_BG_COLOR);
    draw_string_raw(1, footer_row, HINT_TEXT, scr_cols, MUTED_COLOR);

    uint32_t status_len = text_length(status);
    if (status_len > 0 && scr_cols > status_len + 2) {
        draw_string_raw(scr_cols - status_len - 1, footer_row, status, status_len, SUCCESS_COLOR);
    }

    gfx_present();
}
