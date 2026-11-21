#ifndef STRING_H
#define STRING_H

#include "stddef.h"
#include "stdlib.h"

#define PATH_COMPONENT_SIZE 15

int strtok(const char* str, char pattern);
void split_string(const char *path, char parts[][PATH_COMPONENT_SIZE], char pattern, int max_parts);
int strcmp(const char* a, const char* b);
bool memcmp(const void* s1, const void* s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int strlen(const char* c);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);

#endif
