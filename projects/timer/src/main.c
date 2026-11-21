#include "zrn.h"
#include "stdio.h"
#include "gfx.h"
#include "time.h"
#include "video.h"
#include "font.h"

#define WIDTH  1024
#define HEIGHT 768

ZRN_ENTRY
void main(void) {
    gfx_open(WIDTH, HEIGHT);
    font_set_scale(5);
    struct time_now t;

    for(;;) {
        gfx_clear(rgb(0, 0, 0));
        get_time(&t);
        print("%02d:%02d:%02d", t.hours, t.minutes, t.seconds);

        sleep(1000);
    }

    gfx_close();
    return;
}
