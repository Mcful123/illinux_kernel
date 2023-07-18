#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H


uint32_t isBackspace;   //detemines whether backspace is pressed
uint32_t isTab;         //determines whether tab is pressed
uint32_t isEnter;       //determines whether enter is pressed.
uint32_t isControl;     //determines whether ctrl is pressed
uint32_t isLeftShift;   //determines whether shift on the left is pressed
uint32_t isRightShift;  //detemines whether shift on the right is pressed
uint32_t isAlt;         //determines whether alt is pressed
uint32_t isCapsLock;    //determines whether CapsLock is on
uint32_t capsLockRelease;   //determines whether the capsLock button has been released once pressed
uint32_t isNumLock;         //determines whether numsLock is on
uint32_t numLockRelease;    //determines whether the numsLock button has been released once pressed
uint32_t buffer_size;       //buffer_size of the keyboard buffer

uint32_t volatile keyboard_lock;    //lock to determine if terminal driver is operating

#define PIC_KEYBOARD_PORT 0x01
//this array maps setscan 1 to ascii for values where the mapping applies, 0 otherwise
static const unsigned char keyboard_map[128] =
{
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0, 
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', 
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};

//this array maps setscan 1 codes to ascii values when shift is pressed
static const unsigned char keyboard_map_shift[128] =
{
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, 
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', 
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};

char keyboard_buffer[128];

// function to initialize the keyboard and enable irq
void init_keyboard();
// get the input from the keyboard and maps it to ascii and prints the char on the display
void keyboard_handler();
// get the input from the keyboard and determine whether any special keys, like capslock and shift were pressed.
int handle_typing(uint8_t input);



#endif /* _KEYBOARD_H */
