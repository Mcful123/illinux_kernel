#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "terminal_switch.h"

/****************************************KEYBOARD HANDLER************************************/
/* 
 * init_keyboard
 *   DESCRIPTION: initializes the keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void init_keyboard(){
    isBackspace = 0;
    isTab = 0;
    isEnter = 0;
    isControl = 0;
    isLeftShift = 0;
    isRightShift = 0;
    isAlt = 0;
    isCapsLock = 0;
    capsLockRelease = 1;
    isNumLock = 0;
    numLockRelease = 1;
    buffer_size = 0;
    keyboard_lock = 0;              //assume all buttons have not been pressed, and buttons are released when booting
    //memset(keyboard_buffer,'\0', 128);  //clear keyboard_buffer, may not be necessary
    enable_cursor();    //enable the cursor for typing
    //disable_cursor();
    enable_irq(PIC_KEYBOARD_PORT);                       //keyboard is IRQ1, so enable line 1 on master pic
}

/* 
 * keyboard_handler
 *   DESCRIPTION: this is the RTC handler which gets called when the RTC interrupts
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void keyboard_handler(){    
    uint32_t data = inb(0x60);              //this port has data for the keyboard
    char temp = (uint8_t) data;
    int i;
    
    int isSpecial = handle_typing(temp);

    if(isAlt && data >= 0x3B && data <= 0x3D){
        switch_to_terminal(data - 0x3B);
        send_eoi(PIC_KEYBOARD_PORT);
        return;
    }

    if(terminals[current_screen].keyboard_lock == 1){
        send_eoi(PIC_KEYBOARD_PORT);    //if terminal driver is executing, return from interrupt
        return;
    }

    set_video_memory_location(current_screen);
    if(isSpecial){    //check if special buttons are pressed
        if(isEnter){    //enter is pressed
            terminals[current_screen].keyboard_buffer[terminals[current_screen].buffer_length] = '\n';    //add line feed to buffer
            terminals[current_screen].buffer_length++;  //increase size of buffer
            putc('\n'); //put new line feed onto screen
            terminals[current_screen].keyboard_lock = 1;  //let terminal driver read buffer
            
        }

        else if(isBackspace){   //if backspace is pressed
            if(terminals[current_screen].buffer_length){    //if letters printed for the current line
                terminals[current_screen].buffer_length--;  //decrease size of buffer
                //keyboard_buffer[buffer_size] = '\0';
                go_backwards(); //print space in current location
            }
        }
        else if(isTab){ //tab is pressed
            if(terminals[current_screen].buffer_length + 4 <127){   //can tab be allowed in the buffer
                int num = do_tab(); //get how many spaces acually allowed
                for(i = 0; i < num; i++){
                    terminals[current_screen].keyboard_buffer[terminals[current_screen].buffer_length + i] = ' '; //add spaces to the buffer
                }
                terminals[current_screen].buffer_length += num; //increase size of buffer
            }
        }

    }
    else{
        temp = 0;   //set temp to 0 in case 
        if(data < 0x80){ //if lower than x80, button is pressed
            temp = keyboard_map[data]; //map scan code to ascii code
            if(temp >= 'a' && temp <= 'z' && isCapsLock){   //check if capslock is on
                if(!isLeftShift && !isRightShift){
                    temp = temp +('A'-'a');     //if shift is not presssed, change to capital letters
                }  
            }
            else if(temp >= '0' && temp <= '9' && isNumLock){   //if numlock and button is number, do nothing

            } 
            else if(isLeftShift || isRightShift){
                temp = keyboard_map_shift[data]; //if the input character is not defined, keep it to 0
            }   

        }    
        if(temp != 0){  //button pressed is of significant value
            if(isControl && (temp == 'l' || temp == 'L')){ //ctrl + L
                clear();
                for(i=0; i< terminals[current_screen].buffer_length; i++){
                    putc(terminals[current_screen].keyboard_buffer[i]);   //clear the screen and put contents of buffer onto screen again
                }
            }
            else if(terminals[current_screen].buffer_length <127){
                putc(temp); //put character onto console
                terminals[current_screen].keyboard_buffer[terminals[current_screen].buffer_length] = temp;    //add character to buffer
                terminals[current_screen].buffer_length++;  //increase size of buffer
            }

        }
            

    }
    set_video_memory_location(current_task);                       
    send_eoi(PIC_KEYBOARD_PORT);                         //send end of interrupt for IRQ1
}


/* 
 * handle_typing
 *   DESCRIPTION: checks if special buttons are pressed
 *   INPUTS: char input, which is related to set scan code 1
 *   OUTPUTS: none
 *   RETURN VALUE: 1 if special character is pressed, 0 otherwise
 *   SIDE EFFECTS: none
 */
int handle_typing(uint8_t input){
    if(input == 0x0E){  //scan code for backspace being pressed
        isBackspace = 1;
        return 1;
    }
    if(input == 0x0f){  //scan code for tab being pressed
        isTab = 1;
        return 1;
    }
    if(input == 0x1C){  //scan code for enter being pressed
        isEnter = 1;
        return 1;
    } 
    if(input == 0x1d){  //scan code for control being pressed
        isControl = 1;
        return 0;
    }
    if(input == 0x2a){  //scan code for left shift being pressed
        isLeftShift = 1;
        return 0;
    }
    if(input == 0x36){  //scan code for right shift being pressed
        isRightShift = 1;
        return 0;
    }
    if(input == 0x38){  //scan code for alt being pressed
        isAlt = 1;
        return 0;
    }
    if(input == 0x3A){  //scan code for capslock button being pressed
        if(capsLockRelease){
            isCapsLock = !isCapsLock;   //if button is pressed and has been released, set to 0
            capsLockRelease = 0;    //button has not been released when pressed
        }
        return 0;
    }
    if(input == 0x45){  //scan code for numslock button being pressed
        if(numLockRelease){
            isNumLock = !isNumLock; //if button is pressed and has been released, set to 0
            numLockRelease = 0; //button has not been released when pressed
        }
        return 0;
    }

    if(input == 0x8E){  //scan code for backspace being released
        isBackspace = 0;
        return 0;
    }
    if(input == 0x8f){  //scan code for tab being released
        isTab = 0;
        return 0;
    }
    if(input == 0x9C){  //scan code for enter being released
        isEnter = 0;
        return 0;
    }
    if(input == 0x9d){  //scan code for control being released
        isControl = 0;
        return 0;
    }
    if(input == 0xAa){  //scan code for left shift being released
        isLeftShift = 0;
        return 0;
    }
    if(input == 0xB6){  //scan code for right shift being released
        isRightShift = 0;
        return 0;
    }
    if(input == 0xb8){  //scan code for alt being released
        isAlt = 0;
        return 0;
    }
    if(input == 0xbA){  //scan code for capslock being released
        capsLockRelease = 1;
        return 0;
    }
    if(input == 0xc5){  //scan code for numslock being released
        numLockRelease = 1;
        return 0;
    }
    return 0;
}
