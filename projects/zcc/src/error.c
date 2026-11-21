#include "video.h"
#include "stdio.h"
#include "error.h"

bool resolve_error(int err) {
    if (err > 0) {
        print("%CWARNING: %C", rgb(255, 255, 0), rgb(255, 255, 255));
    } else if (err < 0) {
        print("%CERROR: %C", rgb(255, 0, 0), rgb(255, 255, 255));
    }

    switch (err) {
        case NO_ERROR:
            return true;
        case HAS_NOT_RETURN:
            print("Wasn't return\n");
            return true;
    }

    return false;
}
