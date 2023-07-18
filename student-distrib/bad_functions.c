#include "bad_functions.h"

//'bad' function calls all return -1
//they are used for the function pointer tables in a File descriptor

/*
 * bad_read
 *   DESCRIPTION: for system calls that shouldnt be reading always returns negative 1
 *   INPUTS: int32_t fd, void* buf, int32_t nbytes
 *   RETURN VALUE: -1
 */ 
int32_t bad_read(int32_t fd, void* buf, int32_t nbytes) {
    return -1;
}

/*
 * bad_write
 *   DESCRIPTION: for system calls that shouldnt be writing always returns negative 1
 *   INPUTS: int32_t fd, void* buf, int32_t nbytes
 *   RETURN VALUE: -1
 */ 
int32_t bad_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/*
 * bad_open
 *   DESCRIPTION: for system calls that shouldnt be opening always returns negative 1
 *   INPUTS: const uint8_t* filename
 *   RETURN VALUE: -1
 */ 
int32_t bad_open(const uint8_t* filename) {
    return -1;
}

/*
 * bad_close
 *   DESCRIPTION: for system calls that shouldnt be closing always returns negative 1
 *   INPUTS: int32_t fd
 *   RETURN VALUE: -1
 */ 
int32_t bad_close(int32_t fd) {
    return -1;
}
