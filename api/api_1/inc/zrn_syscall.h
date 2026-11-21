#ifndef ZRN_SYSCALL_H
#define ZRN_SYSCALL_H

#include "stdint.h"
#include "stddef.h"
#include "time.h"
#include "mouse.h"

#define SYSCALL_TABLE (*((void***)0x50000))

enum {
    SYS_CLEAR = 0,
    SYS_SLEEP = 1,
    SYS_GET_TIME = 2,
    SYS_DISPLAY_OPEN = 3,
    SYS_DISPLAY_PIXEL = 4,
    SYS_DISPLAY_PRESENT = 5,
    SYS_GETCH = 6,
    SYS_GET_FONT = 7,
    SYS_DISPLAY_FILL = 8,
    SYS_DISPLAY_CLOSE = 9,
    SYS_MKDIR = 10,
    SYS_RMDIR = 11,
    SYS_PUTC = 12,
    SYS_CREATE_FILE = 13,
    SYS_WRITE_FILE = 14,
    SYS_READ_FILE = 15,
    SYS_ARGS = 16,
    SYS_EXIT = 17,
    SYS_IS_MOUSE_EVENT = 18,
    SYS_GET_MOUSE_EVENT = 19,
    SYS_GET_FILE_SIZE = 20
};

typedef void (*sys_clear_t)(void);
typedef void (*sys_sleep_t)(const uint32_t);
typedef void (*sys_get_time_t)(struct time_now* t);
typedef char (*sys_getch_t)(void);
typedef const uint8_t* (*sys_get_font_t)(uint8_t c);
typedef void (*sys_putc_t)(uint8_t ch, uint32_t color);
typedef void (*sys_exit_t)(void);
typedef bool (*sys_is_mouse_event_t)(void);
typedef void (*sys_get_mouse_event_t)(mouse_event_t* event);
typedef int (*sys_get_file_size_t)(const char* filename, size_t* size);

typedef int (*sys_mkdir_t)(char* name);
typedef void (*sys_rmdir_t)(char* name);
typedef void (*sys_create_file_t)(const char* name);
typedef void (*sys_write_file_t)(const char* name, const char* data, uint32_t size);
typedef int32_t (*sys_read_file_t)(const char* name, char* buffer, uint32_t size);

typedef const uint8_t* (*sys_args_t)(void);

typedef bool (*sys_display_open_t)(uint32_t width, uint32_t height);
typedef bool (*sys_display_close_t)(void);
typedef void (*sys_display_pixel_t)(uint32_t x, uint32_t y, uint32_t color);
typedef void (*sys_display_fill_t)(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
typedef void (*sys_display_present_t)(void);

#endif
