#include "zrn.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "video.h"
#include "system.h"
#include <stdint.h>

#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1024
#define NAME_CAPACITY 13

void show_image(const char* name) {
    char content[4096];
    uint8_t* pixel = pixel;

    int32_t readed = read_file(name, &content, sizeof(content));
    int32_t width;
    int32_t height;

    if (readed < 54 || content[0] != 'B' || content[1] != 'M')
        return;

    memcpy(&width, content + 18, 4);
    memcpy(&height, content + 22, 4);

    if (content[0] != 'B') return;

    while (content ) {
        uint8_t r = *pixel++;
        uint8_t g = *pixel++;
        uint8_t b = *pixel++;

        put_pixel();
    }
}

ZRN_ENTRY
void main() {
    char name[NAME_CAPACITY]

    if (!gfx_open(SCREEN_WIDTH, SCREEN_HEIGHT)) return;

    if (args_count() > 0) {
        strncpy(name, args_at(0), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }

    show_image(name);

    return;
}
