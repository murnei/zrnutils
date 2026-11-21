#include "zrn.h"
#include "stdio.h"
#include "string.h"
#include "args.h"
#include "system.h"
#include "compile.h"
#include "error.h"

ZRN_ENTRY
void main() {
    if (args_count() == 0) return;

    const char* arg = args_at(0);
    if (!arg) return;

    static struct buffer_t buf;
    static struct buffer_t output;

    char name[13] = {0};
    memcpy(name, arg, 12);
    name[12] = '\0';

    output.length = 0;
    buf.length = read_file(name, buf.data, BUF_CAPACITY - 1);

    buf.data[buf.length] = '\0';

    if (resolve_error(compiler_main(name, &buf, &output))) {
        write_file("a.out", (const char*)output.data, output.length);
    } else {
        print("Compile error");
    }

    return;
}
