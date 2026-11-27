#include "zrn.h"
#include "zrn_syscall.h"
#include "stdint.h"
#include "stdio.h"
#include "keyboard.h"
#include "gfx.h"
#include "fs.h"
#include "args.h"
#include "time.h"
#include "buffer.h"
#include "view.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define NAME_CAPACITY 64
#define TAB_WIDTH 4

#define KEY_ESC    27

static buffer_t buffer;

static uint8_t key_read(void) {
    char ch;
    for (;;) {
        ch = getch();
        if (ch != 0) return (uint8_t)ch;
        sleep(1);
    }
}

static bool is_printable(uint8_t ch) {
    return ch >= 32 && ch <= 126;
}

static void read_name(char* name) {
    uint32_t length = 0;
    name = '\0';

    for (;;) {
        view_prompt("Open or create file: ", name);
        uint8_t ch = key_read();

        if (ch == '\n') {
            if (length > 0) return;
        } else if (ch == '\b') {
            if (length > 0) name[--length] = '\0';
        } else if (ch > 32 && ch <= 126 && length + 1 < NAME_CAPACITY) {
            name[length++] = (char)ch;
            name[length] = '\0';
        }
    }
}

static void copy_name(char* name, const char* source) {
    uint32_t length = 0;

    while (source[length] && length + 1 < NAME_CAPACITY) {
        name[length] = source[length];
        length++;
    }
    name[length] = '\0';
}

static bool load(const char* name) {
    int32_t count = read_file(name, (char*)buffer.data, BUFFER_CAPACITY - 1);

    if (count < 0) {
        buffer_clear(&buffer);
        return false;
    }

    uint32_t kept = 0;
    for (int32_t i = 0; i < count; i++) {
        uint8_t ch = buffer.data[i];
        if (ch == '\t') {
            for (uint32_t t = 0; t < TAB_WIDTH && kept < BUFFER_CAPACITY - 1; t++) {
                buffer.data[kept++] = ' ';
            }
        } else if (ch == '\n' || is_printable(ch)) {
            if (kept < BUFFER_CAPACITY - 1) {
                buffer.data[kept++] = ch;
            }
        }
    }

    buffer.length = kept;
    buffer.cursor = 0;
    return true;
}

static void save(const char* name, bool exists) {
    if (exists) {
        remove_directory(name);
    }
    create_file(name);
    write_file(name, (const char*)buffer.data, buffer.length);
}

static void insert_tab(void) {
    for (uint32_t i = 0; i < TAB_WIDTH; i++) {
        buffer_insert(&buffer, ' ');
    }
}

static bool edit(const char* name, bool* exists) {
    const char* status = *exists ? "Loaded" : "New file";
    bool modified = false;

    for (;;) {
        view_editor(&buffer, name, status);

        uint8_t ch = key_read();
        status = "";

        if (ch == KEY_ESC) {
            return !modified;
        } else if (ch == KEY_LEFT) {
            buffer_move_left(&buffer);
        } else if (ch == KEY_RIGHT) {
            buffer_move_right(&buffer);
        } else if (ch == KEY_UP) {
            buffer_move_up(&buffer);
        } else if (ch == KEY_DOWN) {
            buffer_move_down(&buffer);
        } else if (ch == KEY_HOME) {
            buffer_move_home(&buffer);
        } else if (ch == KEY_END) {
            buffer_move_end(&buffer);
        } else if (ch == KEY_DELETE) {
            if (buffer_delete(&buffer)) modified = true;
        } else if (ch == '\b') {
            if (buffer_backspace(&buffer)) modified = true;
        } else if (ch == '\t') {
            insert_tab();
            modified = true;
        } else if (ch == '\n') {
            if (buffer_line_is(&buffer, ":wq")) {
                buffer_drop_line(&buffer);
                if (buffer.cursor > 0 && buffer.data[buffer.cursor - 1] == '\n') {
                    buffer_backspace(&buffer);
                }
                save(name, *exists);
                *exists = true;
                return true;
            }
            if (buffer_line_is(&buffer, ":w")) {
                buffer_drop_line(&buffer);
                if (buffer.cursor > 0 && buffer.data[buffer.cursor - 1] == '\n') {
                    buffer_backspace(&buffer);
                }
                save(name, *exists);
                *exists = true;
                modified = false;
                status = "Saved!";
                continue;
            }
            if (buffer_line_is(&buffer, ":q")) {
                return false;
            }
            if (!buffer_insert(&buffer, '\n')) {
                status = "Buffer full!";
            } else {
                modified = true;
            }
        } else if (is_printable(ch)) {
            if (!buffer_insert(&buffer, ch)) {
                status = "Buffer full!";
            } else {
                modified = true;
            }
        }
    }
}

ZRN_ENTRY
void main(void) {
    char name[NAME_CAPACITY];

    if (!gfx_open(SCREEN_WIDTH, SCREEN_HEIGHT)) return;
    if (args_count() > 0) copy_name(name, args_at(0));
    else read_name(name);

    buffer_clear(&buffer);

    bool exists = load(name);
    bool saved = edit(name, &exists);

    gfx_close();

    print(saved ? "Saved %s\n" : "Closed %s\n", name);
}
