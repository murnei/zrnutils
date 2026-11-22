#include "zrn.h"
#include "gfx.h"
#include "cursor.h"
#include "video.h"
#include "mouse.h"
#include "workspace.h"
#include "windows.h"
#include "config.h"
#include "images.h"
#include "taskbar.h"

static void init() {
    set_cursor_visible(false);
    if (!gfx_open(SCREEN_WIDTH, SCREEN_HEIGHT))
        return;

    window_t window;
    window.title = "Explorer";
    window.x = 100;
    window.y = 100;
    window.width = 350;
    window.height = 250;
    window.color = rgb(255, 255, 0);

    workspace_init();
    taskbar_init();
    mouse_init();
    create_window(&window);
    window.title = "Femboy futa house";
    window.x = 150;
    window.y = 150;
    create_window(&window);
}

static void GUI_update(void) {
    bool is_dirty = false;
    is_dirty |= is_windows_dirty();
    is_dirty |= mouse_update();
    is_dirty |= is_taskbar_dirty();

    if (is_dirty)
        gfx_present();
}

ZRN_ENTRY
void main() {
    init();

    while (1) {
        GUI_update();
    }

    return;
}
