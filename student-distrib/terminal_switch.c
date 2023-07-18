#include "terminal_switch.h"
#include "keyboard.h"
#include "lib.h"
#include "paging.h"
#include "system_calls.h"
#include "handlers.h"

/* 
 * switch_to_terminal
 *   DESCRIPTION: switches to terminal described by number
 *   INPUTS: number of terminal to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes terminal
 */

void switch_to_terminal(uint32_t terminal_num){
    if(terminal_num > 2 || terminal_num == current_screen) //if terminal_num is invalid, dont do anything
        return;
    


    update_cursor(terminals[terminal_num].screen_x, terminals[terminal_num].screen_y); //update cursor of new terminal

    move_video_memory(terminal_num);    //change the video memory
    current_screen = terminal_num;
    //switch_to_task(terminal_num);
    change_vidmap_memory(current_task); //change the vidmap settings
    return;
}

/* 
 * switch_to_task
 *   DESCRIPTION: switches to task described by number
 *   INPUTS: number of task to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes task
 */

void switch_to_task(uint32_t task_num){
    if(task_num > 2 || task_num == current_task)
        return;

    if(curr_PCBs[current_task]->isVidMapSet) //unset vidmap memory if current user program is not set
        unset_vidmap_memory();

    uint32_t temp_ebp, temp_esp; //variables to store esp/ebp
    // saving current task stuff
    asm volatile ("movl %%ebp, %0\n"
		         : "=g"(temp_ebp)
		         :
                 :"ebp");
    
    asm volatile ("movl %%esp, %0\n"
		         : "=g"(temp_esp)
		         :
                 :"esp");

    curr_esp0s[current_task] = tss.esp0; 
    curr_esps[current_task] = temp_esp;
    curr_ebps[current_task] = temp_ebp; //store important values of current task

    current_task = task_num;    //change the value of the current task

    if(curr_PCBs[task_num] == NULL){ 
        //send_eoi(PIC_KEYBOARD_PORT);
        // execute((const uint8_t*)"shell");
        set_video_memory_location(current_task);
        isBaseTask = 1;
        send_eoi(PIC_PIT_PORT);
        execute((const uint8_t*)"shell"); //if the thing is null, execute shell

        return;
    }
    

    temp_esp = curr_esps[current_task];
    temp_ebp = curr_ebps[current_task];

    tss.esp0 = curr_esp0s[current_task]; //change tss value

    asm volatile ("movl %0 , %%ebp\n"
		         : "=g"(temp_ebp)
		         :
                 :"ebp");
    
    asm volatile ("movl %0 , %%esp\n"
		         : "=g"(temp_esp)
		         :
                 :"esp");  //change ebp and esp

    set_paging_for_user_process(curr_pids[current_task]); //set paging for the current task

    set_video_memory_location(current_task); //change the pointer of video_mem

    if(curr_PCBs[current_task]->isVidMapSet){
        set_vidmap_memory();
        change_vidmap_memory(current_task); //change vidmap settings
    }

    return; 
}
