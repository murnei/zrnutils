#ifndef MOUSE_H
#define MOUSE_H

#include "stdint.h"

typedef struct {
    int16_t dx;
    int16_t dy;
    uint8_t button;
} mouse_event_t;

bool is_mouse_event(void);
void get_mouse_event(mouse_event_t* event);

#endif
