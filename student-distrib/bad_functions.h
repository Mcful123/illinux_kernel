#include "types.h"
#ifndef _BAD_FUNCTIONS_H
#define _BAD_FUNCTIONS_H

//'bad' function calls all return -1
//they are used for the function pointer tables in a File descriptor
int32_t bad_read(int32_t fd, void* buf, int32_t nbytes);
int32_t bad_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t bad_open(const uint8_t* filename);
int32_t bad_close(int32_t fd);

#endif /* _BAD_FUNCTIONS_H */
