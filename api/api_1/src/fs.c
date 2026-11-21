#include "stdint.h"
#include "zrn_syscall.h"

int make_directory(const char* name) {
    return ((sys_mkdir_t)SYSCALL_TABLE[SYS_MKDIR])((char*)name);
}

void remove_directory(const char* name) {
    ((sys_rmdir_t)SYSCALL_TABLE[SYS_RMDIR])((char*)name);
}

void create_file(const char* name) {
    ((sys_create_file_t)SYSCALL_TABLE[SYS_CREATE_FILE])(name);
}

void write_file(const char* name, const char* data, uint32_t size) {
    ((sys_write_file_t)SYSCALL_TABLE[SYS_WRITE_FILE])(name, data, size);
}

int32_t read_file(const char* name, char* buffer, uint32_t size) {
    return ((sys_read_file_t)SYSCALL_TABLE[SYS_READ_FILE])(name, buffer, size);
}

int get_file_size(const char* filename, size_t *size) {
    return ((sys_get_file_size_t)SYSCALL_TABLE[SYS_GET_FILE_SIZE])(filename, size);
}
