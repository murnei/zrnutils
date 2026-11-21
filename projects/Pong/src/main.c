#include "zrn.h"
#include "stdio.h"
#include "gfx.h"
#include "game.h"

static void init(void) {
    gfx_open(1024, 768);
}

ZRN_ENTRY
void main() {
    init();

    game_main();

    for (;;)
        ;

    gfx_close();
    return;
}
