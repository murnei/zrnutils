#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "stdint.h"

void workspace_init(void);
void workspace_manager(int32_t x, int32_t y, bool pressed, bool realised);

#endif
