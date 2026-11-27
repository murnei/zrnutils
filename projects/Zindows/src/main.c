#include "zrn.h"
#include "system.h"
#include "gfx.h"
#include "cursor.h"
#include "video.h"
#include "time.h"
#include "mouse.h"
#include "workspace.h"
#include "windows.h"
#include "config.h"
#include "images.h"
#include "taskbar.h"

static void init(void) {
    set_cursor_visible(false);
    if (!gfx_open(SCREEN_WIDTH, SCREEN_HEIGHT))
        return;

    workspace_init();
    taskbar_init();
    windows_init();
    mouse_init();

    render_bmp("/home/pics/output.bmp", 50, 50);
}

static void GUI_update(void) {
    bool is_dirty = false;

    mouse_update();

    is_dirty |= workspace_is_dirty();
    is_dirty |= is_windows_dirty();
    is_dirty |= mouse_is_dirty();
    is_dirty |= is_taskbar_dirty();

    if (is_dirty)
        gfx_present();
}

ZRN_ENTRY
void main() {
    init();

    while (1) {
        GUI_update();
        taskbar_update_clocks();
        // sleep(2);
    }

    return;
}
