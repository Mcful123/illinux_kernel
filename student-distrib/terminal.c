#include "terminal.h"
#include "terminal_switch.h"
/* 
 * terminal_read
 *   DESCRIPTION: takes keyboard input and puts into a buffer
 *   INPUTS: void* buf - a pointer to a buffer of chars, int nbytes - how many chars to copy
 *   OUTPUTS: none
 *   RETURN VALUE: return how many bytes put into buf
 *   SIDE EFFECTS: modifies contents pointed by buf
 */

int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    while(terminals[current_task].keyboard_lock == 0){
        //while enter has not been pressed, do nothing
    }

    if(buf == NULL){    //check if buf is not NULL
        terminals[current_task].keyboard_lock = 0;
        return -1;
    }   
    int32_t min;
    if(nbytes > terminals[current_task].buffer_length){
        min = terminals[current_task].buffer_length;
    }
    else{
        min = nbytes;
    }   //get min size between nbytes and buffer_size
    memcpy(buf, terminals[current_task].keyboard_buffer, min);  //copy contents from  keyboard_buffer into buf
    
    memset(terminals[current_task].keyboard_buffer,'\0', 128);
    terminals[current_task].buffer_length = 0;    //set buffer_size to 0

    terminals[current_task].keyboard_lock = 0;  //allow interrputs to occur
    return min; //return number of things written to buf

}

/* 
 * terminal_write
 *   DESCRIPTION: takes contents of buffer onto the screen
 *   INPUTS: void* buf - a pointer to a buffer of chars, int nbytes - how many chars to put onto screen
 *   OUTPUTS: none
 *   RETURN VALUE: return how many bytes put into screen
 *   SIDE EFFECTS: modifies contents of screen
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    
    terminals[current_task].keyboard_lock = 1;  //dont allow interrupts to occur
    if(nbytes < 0 || buf == NULL){
        terminals[current_task].keyboard_lock = 0;
        return -1;  //if invalid inputs, return -1
    }
    int i;
    uint8_t temp;
    int32_t count = 0;
    set_video_memory_location(current_task);
    for(i = 0; i< nbytes; i++){
        temp = ((uint8_t*) buf)[i]; //get current index into buffer
        putc(temp); //put character onto the screen
        count++;   //count how many characters on screen
    }
    terminals[current_task].keyboard_lock = 0;

    return count;       //return how many characters printed
        
}

/* 
 * terminal_open
 *   DESCRIPTION: doesnt do anything
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_open(){
    return 0;
}

/* 
 * terminal_close
 *   DESCRIPTION: doesnt do anything
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_close(){
    return 0;
}
