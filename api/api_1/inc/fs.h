#ifndef FS_H
#define FS_H

#include "stdint.h"
#include "stddef.h"

#ifndef FS_OK
#define FS_OK            0
#define FS_ERR_NOFS     -1
#define FS_ERR_EXISTS   -2
#define FS_ERR_NOTFOUND -3
#define FS_ERR_NOTDIR   -4
#define FS_ERR_NOTEMPTY -5
#define FS_ERR_NOSPACE  -6
#define FS_ERR_IO       -7
#define FS_ERR_ISDIR    -8
#define FS_ERR_DIRFULL  -9
#define FS_NOT_READY   -10
#endif

int make_directory(const char* name);
void remove_directory(const char* name);
void create_file(const char* name);
void write_file(const char* name, const char* data, uint32_t size);
int32_t read_file(const char* name, char* buffer, uint32_t size);
int get_file_size(const char* filename, size_t *size);

#endif
