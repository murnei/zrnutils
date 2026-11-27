#include "stdint.h"
#include "stdlib.h"
#include "fs.h"
#include "gfx.h"

#define DATA_OFFSET 54

static uint16_t read_u16(const uint8_t* p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read_u32(const uint8_t* p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

void render_bmp(const char* path, int32_t cx, int32_t cy) {
    size_t filesize;
    if (get_file_size(path, &filesize) != 0) return;

    uint8_t* buffer = kmalloc(filesize);
    if (!buffer) return;
    int32_t offset;

    if (read_file(path, (char*)buffer, filesize) == 0) return;

    if (buffer[0] != 'B' || buffer[1] != 'M') {
        kfree(buffer);
        return;
    }

    uint32_t data_offset = read_u32(buffer + 10);
    int32_t  width       = (int32_t)read_u32(buffer + 18);
    int32_t  height      = (int32_t)read_u32(buffer + 22);
    uint16_t bpp         = read_u16(buffer + 28);
    if (bpp != 24 && bpp != 32) {
        kfree(buffer);
        return;
    }
    uint32_t bytes_per_pixel = bpp / 8;
    uint32_t row_size = ((width * bytes_per_pixel + 3) / 4) * 4;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            offset = data_offset + (y * row_size) + (x * bytes_per_pixel);
            uint8_t b = buffer[offset];
            uint8_t g = buffer[offset+1];
            uint8_t r = buffer[offset+2];
            uint32_t color = 0xFF000000 | (r << 16) | (g << 8) | b;
            gfx_pixel(cx+x, cy+(height-1-y), color);
        }
    }
    kfree(buffer);
}
