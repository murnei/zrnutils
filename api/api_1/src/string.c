#include "stdint.h"
#include "stddef.h"
#include "string.h"

int strlen(const char* c) {
    int i = 0;

    while(c[i] != '\0') {
        i++;
    }

    return i;
}

int strcmp(const char* a, const char* b) {
    while (*a == *b && *a != '\0') {
        a++;
        b++;
    }

    return (unsigned char)*a - (unsigned char)*b;
}

bool memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* a = s1;
    const uint8_t* b = s2;

    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t* d = dest;
    const uint8_t* s = src;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
	}

    return dest;
}

char* strcpy(char* dest, const char* src) {
    char* original = dest;

    while ((*dest++ = *src++) != '\0')
      ;

    return original;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;

    while (n > 0 && *src != '\0') {
        *d++ = *src++;
        n--;
    }

    while (n > 0) {
        *d++ = '\0';
        n--;
    }

    return dest;
}

char* strcat(char* dest, const char* src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

int strtok(const char* str, char pattern) {
    int i = 0;

    while (str[i] != '\0' && str[i] != pattern) {
        i++;
    }

    return i;
}

void split_string(const char *path, char parts[][PATH_COMPONENT_SIZE], char pattern, int max_parts) {
    int part = 0;

    while (*path != '\0' && part < max_parts) {
        if (*path == pattern) {
            path++;
            continue;
        }

        int i = 0;

        while (*path != '\0' && *path != pattern) {
            if (i < PATH_COMPONENT_SIZE - 1) {
                parts[part][i++] = *path;
            }

            path++;
        }

        parts[part][i] = '\0';
        part++;
    }

    while (part < max_parts) {
        parts[part][0] = '\0';
        part++;
    }
}
