#ifndef _TERMINAL_SWITCH_H
#define _TERMINAL_SWITCH_H

#include "types.h"

volatile uint32_t current_screen; //holds the value of the current_screen

volatile uint32_t current_task; //holds the value of the current_task


// information for every terminal
struct terminal_info{
    uint8_t top_line_buffer[160]; 
    char keyboard_buffer[128];
    int buffer_length;
    int keyboard_lock;
    int screen_x;
    int screen_y;
};

typedef struct terminal_info terminal_info;

terminal_info terminals[3]; //array of information for terminals
// switches to terminal based on number
void switch_to_terminal(uint32_t terminal_num);
// sswitches to task based on number
void switch_to_task(uint32_t task_num);

#endif /* _TERMINAL_SWITCH_H */
