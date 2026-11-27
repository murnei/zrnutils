#include "CloseGL.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "gfx.h"

#define BACKGROUND_COLOR 0xFF000000
#define FRAME_DELAY 2000000
#define ANGLE_STEP 2

typedef struct {
    float x;
    float y;
    float z;
} vertex_t;

static const vertex_t cube_vertices[8] = {
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f}
};

static const unsigned char cube_edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

static inline void put_px_fast(uint32_t* back, int w, int h, int x, int y, uint32_t color) {
    if ((unsigned int)x < (unsigned int)w && (unsigned int)y < (unsigned int)h) {
        back[y * w + x] = color;
    }
}

static void rect_internal(uint32_t* back, int scr_w, int scr_h, int x, int y, int width, int height, uint32_t color, bool notify_dirty) {
    if (back == nullptr || width <= 0 || height <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + width > scr_w ? scr_w : x + width;
    int y1 = y + height > scr_h ? scr_h : y + height;

    int draw_w = x1 - x0;
    int draw_h = y1 - y0;
    if (draw_w <= 0 || draw_h <= 0) return;

    uint32_t* row = &back[y0 * scr_w + x0];
    for (int py = 0; py < draw_h; py++) {
        uint32_t* dst = row;
        uint32_t count = (uint32_t)draw_w;
        __asm__ volatile("cld; rep stosl" : "+D"(dst), "+c"(count) : "a"(color) : "memory");
        row += scr_w;
    }

    if (notify_dirty) {
        gfx_mark_dirty((uint32_t)y0, (uint32_t)y1);
    }
}

static void line_internal(uint32_t* back, int w, int h, int x0, int y0, int x1, int y1, uint32_t color, int* min_y, int* max_y) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        if ((unsigned int)x0 < (unsigned int)w && (unsigned int)y0 < (unsigned int)h) {
            back[y0 * w + x0] = color;
            if (y0 < *min_y) *min_y = y0;
            if (y0 > *max_y) *max_y = y0;
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void render_line(int x0, int y0, int x1, int y1, uint32_t color) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();
    int min_y = h;
    int max_y = -1;

    line_internal(back, w, h, x0, y0, x1, y1, color, &min_y, &max_y);

    if (min_y <= max_y) {
        gfx_mark_dirty((uint32_t)min_y, (uint32_t)(max_y + 1));
    }
}

static vertex_t rotate_vertex(vertex_t v, int ax, int ay, int az) {
    float sax = sin(ax), cax = cos(ax);
    float say = sin(ay), cay = cos(ay);
    float saz = sin(az), caz = cos(az);
    float x, y, z;

    y = v.y * cax - v.z * sax;
    z = v.y * sax + v.z * cax;
    v.y = y;
    v.z = z;

    x = v.x * cay + v.z * say;
    z = -v.x * say + v.z * cay;
    v.x = x;
    v.z = z;

    x = v.x * caz - v.y * saz;
    y = v.x * saz + v.y * caz;
    v.x = x;
    v.y = y;

    return v;
}

void render_cube(int cx, int cy, int size, uint32_t color, int ax, int ay, int az) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();
    int min_y = h;
    int max_y = -1;

    float half = size / 2.0f;
    float dist = size * 3.0f;
    int px[8];
    int py[8];

    for (int i = 0; i < 8; i++) {
        vertex_t v = cube_vertices[i];
        v.x *= half;
        v.y *= half;
        v.z *= half;

        v = rotate_vertex(v, ax, ay, az);

        float k = dist / (dist + v.z);
        px[i] = cx + (int)(v.x * k);
        py[i] = cy + (int)(v.y * k);
    }

    for (int i = 0; i < 12; i++) {
        int a = cube_edges[i][0];
        int b = cube_edges[i][1];
        line_internal(back, w, h, px[a], py[a], px[b], py[b], color, &min_y, &max_y);
    }

    if (min_y <= max_y) {
        gfx_mark_dirty((uint32_t)min_y, (uint32_t)(max_y + 1));
    }
}

void render_cube_spin(int cx, int cy, int size, uint32_t color) {
    int angle = 0;
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();

    while (gfx_is_open()) {
        rect_internal(back, w, h, 0, 0, w, h, BACKGROUND_COLOR, true);
        render_cube(cx, cy, size, color, angle, angle * 2, angle / 2);
        gfx_present();

        angle = (angle + ANGLE_STEP) % 360;

        for (volatile int i = 0; i < FRAME_DELAY; i++) {
        }
    }
}

void render_triangle(int cx, int cy, int size, uint32_t color) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr || size <= 0) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();

    for (int i = 0; i < size; i++) {
        put_px_fast(back, w, h, cx + i, cy + i, color);
        put_px_fast(back, w, h, cx - i, cy + i, color);
    }

    rect_internal(back, w, h, cx - size, cy + size, 2 * size + 1, 1, color, false);

    int top = cy < 0 ? 0 : cy;
    int bottom = (cy + size + 1) > h ? h : (cy + size + 1);
    if (top < bottom) gfx_mark_dirty((uint32_t)top, (uint32_t)bottom);
}

void render_triangle_fill(int cx, int cy, int size, uint32_t color) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr || size <= 1) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();

    for (int y = 1; y < size; y++) {
        rect_internal(back, w, h, cx - y + 1, cy + y, 2 * y - 1, 1, color, false);
    }

    int top = (cy + 1) < 0 ? 0 : (cy + 1);
    int bottom = (cy + size) > h ? h : (cy + size);
    if (top < bottom) gfx_mark_dirty((uint32_t)top, (uint32_t)bottom);
}

void render_square(int cx, int cy, int size, uint32_t color) {
    render_rectangle(cx, cy, size, size, color);
}

void render_square_fill(int cx, int cy, int size, uint32_t color) {
    render_rectangle_fill(cx, cy, size, size, color);
}

static inline void circle_points_fast(uint32_t* back, int w, int h, int cx, int cy, int x, int y, uint32_t color) {
    put_px_fast(back, w, h, cx + x, cy + y, color);
    put_px_fast(back, w, h, cx - x, cy + y, color);
    put_px_fast(back, w, h, cx + x, cy - y, color);
    put_px_fast(back, w, h, cx - x, cy - y, color);

    put_px_fast(back, w, h, cx + y, cy + x, color);
    put_px_fast(back, w, h, cx - y, cy + x, color);
    put_px_fast(back, w, h, cx + y, cy - x, color);
    put_px_fast(back, w, h, cx - y, cy - x, color);
}

void render_circle(int cx, int cy, int radius, uint32_t color) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr || radius < 0) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (x <= y) {
        circle_points_fast(back, w, h, cx, cy, x, y, color);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }

    int top = (cy - radius) < 0 ? 0 : (cy - radius);
    int bottom = (cy + radius + 1) > h ? h : (cy + radius + 1);
    if (top < bottom) gfx_mark_dirty((uint32_t)top, (uint32_t)bottom);
}

void render_rectangle(int cx, int cy, int width, int height, uint32_t color) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr || width <= 0 || height <= 0) return;

    int w = (int)gfx_width();
    int h = (int)gfx_height();

    rect_internal(back, w, h, cx, cy, width, 1, color, false);
    if (height > 1) {
        rect_internal(back, w, h, cx, cy + height - 1, width, 1, color, false);
    }
    if (height > 2) {
        rect_internal(back, w, h, cx, cy + 1, 1, height - 2, color, false);
        if (width > 1) {
            rect_internal(back, w, h, cx + width - 1, cy + 1, 1, height - 2, color, false);
        }
    }

    int top = cy < 0 ? 0 : cy;
    int bottom = (cy + height) > h ? h : (cy + height);
    if (top < bottom) gfx_mark_dirty((uint32_t)top, (uint32_t)bottom);
}

void render_rectangle_fill(int cx, int cy, int width, int height, uint32_t color) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr) return;

    rect_internal(back, (int)gfx_width(), (int)gfx_height(), cx, cy, width, height, color, true);
}

void render_rectangle_xor(int cx, int cy, int width, int height) {
    uint32_t* back = gfx_get_backbuffer();
    if (back == nullptr || width <= 0 || height <= 0) return;

    int scr_w = (int)gfx_width();
    int scr_h = (int)gfx_height();

    int x0 = cx;
    int y0 = cy;
    int x1 = cx + width - 1;
    int y1 = cy + height - 1;

    if (x1 < 0 || y1 < 0 || x0 >= scr_w || y0 >= scr_h) return;

    int draw_x0 = x0 < 0 ? 0 : x0;
    int draw_y0 = y0 < 0 ? 0 : y0;
    int draw_x1 = x1 >= scr_w ? scr_w - 1 : x1;
    int draw_y1 = y1 >= scr_h ? scr_h - 1 : y1;

    if (y0 >= 0 && y0 < scr_h) {
        uint32_t* top_row = &back[y0 * scr_w];
        for (int x = draw_x0; x <= draw_x1; x++) {
            top_row[x] ^= 0x00FFFFFF;
        }
    }

    if (y1 >= 0 && y1 < scr_h && y1 != y0) {
        uint32_t* bottom_row = &back[y1 * scr_w];
        for (int x = draw_x0; x <= draw_x1; x++) {
            bottom_row[x] ^= 0x00FFFFFF;
        }
    }

    int vert_start = draw_y0 == y0 ? draw_y0 + 1 : draw_y0;
    int vert_end   = draw_y1 == y1 ? draw_y1 - 1 : draw_y1;

    bool draw_left  = (x0 >= 0 && x0 < scr_w);
    bool draw_right = (x1 >= 0 && x1 < scr_w && x1 != x0);

    uint32_t* row = &back[vert_start * scr_w];
    for (int y = vert_start; y <= vert_end; y++) {
        if (draw_left)  row[x0] ^= 0x00FFFFFF;
        if (draw_right) row[x1] ^= 0x00FFFFFF;
        row += scr_w;
    }
	gfx_mark_dirty((uint32_t)draw_y0, (uint32_t)(draw_y1 + 1));
}
