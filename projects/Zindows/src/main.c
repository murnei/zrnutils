#include "zrn.h"
#include "gfx.h"
#include "cursor.h"
#include "video.h"
#include "mouse.h"
#include "workspace.h"
#include "windows.h"
#include "config.h"

static void init() {
    set_cursor_visible(false);
    if (!gfx_open(SCREEN_WIDTH, SCREEN_HEIGHT))
        return;

    window_t window;
    window.title = "Explorer";
    window.x = 0;
    window.y = 0;
    window.width = 350;
    window.height = 250;
    window.color = rgb(255, 255, 0);

    workspace_init();
    create_window(&window);
    mouse_init();
}

static void GUI_update(void) {
    bool is_dirty = false;
    is_dirty |= mouse_update();
    is_dirty |= is_windows_dirty();

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
