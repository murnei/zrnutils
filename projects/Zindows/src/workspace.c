#include "config.h"
#include "gfx.h"
#include "video.h"
#include "stdint.h"
#include "windows.h"

void workspace_init(void) {
    gfx_fill(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, rgb(83, 109, 254));
}

void workspace_manager(int32_t x, int32_t y, bool pressed, bool realised) {
    if (windows_manager_handle_mouse(x, y, pressed, realised))
        return;
}
