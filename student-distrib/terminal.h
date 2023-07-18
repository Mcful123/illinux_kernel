#include "keyboard.h"
#include "lib.h"

#ifndef _TERMINAL_H
#define _TERMINAL_H

//reads input from the keyboard and puts it into the buffer
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
//takes string from buf and prints it onto the screen
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
//doesnt do anything, needed for checkpoint
int32_t terminal_open(void);
//doesnt do anything, needed for checkpoint
int32_t terminal_close(void);

#endif /* _TERMINAL_H */
