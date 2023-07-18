#include "system_calls.h"
#include "paging.h"
#include "file_system.h"
#include "x86_desc.h"
#include "terminal.h"
#include "bad_functions.h"
#include "handlers.h"
#include "terminal_switch.h"

#define VIDEO  0xB8000      //video location
#define VIDEO_TASK1 0xB9000
#define VIDEO_TASK2 0xBA000
#define VIDEO_TASK3 0xBB000
#define VIDEO_MAP1 0xBC000
#define VIDEO_MAP2 0xBD000
#define VIDEO_MAP3 0xBE000

// static uint32_t VIDEO_TASK[3] = {VIDEO_TASK1, VIDEO_TASK2, VIDEO_TASK3};
static uint32_t VIDEO_MAP[3] = {VIDEO_MAP1, VIDEO_MAP2, VIDEO_MAP3};

// Create the function pointer tables
fops_t stdin_table = {&bad_open, &bad_close, &terminal_read, &bad_write};
fops_t stdout_table = {&bad_open, &bad_close, &bad_read, &terminal_write};
fops_t rtc_table = {&rtc_open, &rtc_close, &rtc_read, &rtc_write};
fops_t ffil = {&f_open, &f_close, &f_read, &f_write};
fops_t fdir  = {&dir_open, &dir_close, &dir_read, &dir_write};



//execute 
/* 
 * execute
 *   DESCRIPTION: takes in a string that is for an executable
 *   INPUTS: const uint8_t* command - a pointer to a buffer of chars
 *   OUTPUTS: creates another user program to run
 *   RETURN VALUE: whether the execute worked properly
 *   SIDE EFFECTS: runs another user program
 */

int32_t execute(const uint8_t* command){
    if(command == NULL)
        return -1;

    uint8_t main_file[128];     //main file name
    uint8_t side_args[128];     //other arguments in command
    uint32_t main_file_idx = 0;     //cur index into main file 
    uint32_t side_args_idx = 0;     //cur index into side_args
    uint8_t* string_parser = (uint8_t*) command;   // pointer to parse command

    dentry_t exec_dentry;       // dentry for the file name 

    //check if file name is present
    while(*string_parser == ' '){
        if(*string_parser == '\n' || *string_parser == '\0')
            return -1;
        string_parser++;
    }

    // get the file name
    while(*string_parser != ' ' && *string_parser != '\n' && *string_parser != '\0'){
        main_file[main_file_idx] = *string_parser;
        main_file_idx++;
        string_parser++;
    }
    main_file[main_file_idx] = '\0';

    while(*string_parser == ' '){
        if(*string_parser == '\n' || *string_parser == '\0')
            return -1;
        string_parser++;
    }

    // get the rest of the arguments
    while(*string_parser != '\n' && *string_parser != '\0'){
        side_args[side_args_idx] = *string_parser;
        side_args_idx++;
        string_parser++;
    }

    side_args[side_args_idx] = '\0';
    // check if name is file system

    if(read_dentry_by_name(main_file, &exec_dentry) == -1){
        //printf("75");
        return -1;
    }

    uint8_t buf_4_bytes[4];

    // read in the first 4 Bytes to check for executable magic numbers
    if(read_data(exec_dentry.inode_num, 0, buf_4_bytes, 4) == -1) {
        //printf("83");
	    return -1;
    }



    //check for the magic numbers at the beginning of the file to check for executable
    if(buf_4_bytes[0] != 0x7f || buf_4_bytes[1] != 0x45 || buf_4_bytes[2] != 0x4c || buf_4_bytes[3] != 0x46){
        //printf("91");
        return -1;
    }

    uint32_t pid_map_cpy = processes_map; //make copy of the processes_map

    uint32_t new_pid = 3;
    uint32_t mask = 0x8;

    if(isBaseTask){
        new_pid = 0;
        mask = 0x1;
        isBaseTask = 0;
    }

    // set the correct pid value
    while(pid_map_cpy & mask){

	    new_pid++;
        
        pid_map_cpy = pid_map_cpy>>1;

    }

    // cant have more than 6 processes
    if(new_pid >=9)
        return -1;

    processes_map = processes_map | (1<<new_pid);

    // Set up paging 
    set_paging_for_user_process(new_pid);

    
    // load file into page
    read_data(exec_dentry.inode_num, 0, (void *)0x08048000, 0x3B8000); //these numbers given to us by document, the 2nd number is 4MB - offset of starting data

    // set start address of PCB
    PCB_begin* PCB_start =  (PCB_begin*) (0x800000 - (new_pid+1)*(0x2000)); // 8MB - pid+1 * 8kb

    // int i;

    file_descriptor_entry empty_fd = {NULL, -1, -1, 0};

    //init fd array to empty vals
    init_fda(PCB_start, empty_fd);
 
    //Add stdin fd into first position in the file desc array this is given in the documentation:
    PCB_start->file_descriptor_array[0].inode = -1;
    PCB_start->file_descriptor_array[0].flags = 1;
    PCB_start->file_descriptor_array[0].file_position = 0;
    PCB_start->file_descriptor_array[0].file_operations_table = &stdin_table;

    //Add stdout fd into second position in the file desc array this is given in the documentation:
    PCB_start->file_descriptor_array[1].inode = -1;
    PCB_start->file_descriptor_array[1].flags = 1;
    PCB_start->file_descriptor_array[1].file_position = 0;
    PCB_start->file_descriptor_array[1].file_operations_table = &stdout_table;
    
    //create and initialize PCB struct with fd array
    PCB_start->prev_pid = curr_pids[current_task];
    PCB_start->prev_esp0 = curr_esp0s[current_task];
    PCB_start->prev_ss0 = curr_ss0s[current_task];
    PCB_start->PCB_begin_prev = (struct PCB_begin*) (curr_PCBs[current_task]);
    PCB_start->fs_offset = 0;
    PCB_start->isVidMapSet = 0;

    strncpy((int8_t*) PCB_start->side_args, (int8_t*) side_args, 128);

    uint32_t curr_ebp;  //current base pointer variable

    //get the ebp value and set curr_ebp
    asm volatile ("movl %%ebp, %0\n"
		         : "=g"(curr_ebp)
		         :
                 :"ebp", "ebx");
    
    PCB_start->exec_ebp = curr_ebp; // store the current ebp value

    //set global values
    curr_pids[current_task] = new_pid;
    curr_esp0s[current_task] = 0x800000 - (new_pid)*(0x2000)-4; //set stack to 4 minus the initial point 8MB - 2kB, 
    curr_ss0s[current_task] = KERNEL_DS;
    curr_PCBs[current_task] = PCB_start;
 
    

    tss.ss0 = curr_ss0s[current_task];

    tss.esp0 = curr_esp0s[current_task];

    //set iret values
    uint32_t iret_ds = USER_DS & 0x0000FFFF;
    uint32_t iret_esp = 0x083FFFFF - 3; //set to 4 minus the bottom of the page
    uint32_t iret_cs = USER_CS & 0x0000FFFF;
    uint32_t iret_eip;

    if(read_data(exec_dentry.inode_num, 24, &iret_eip, 4) == -1) //get eip from the file
	    return 1;
    //push the 5 values for the iret switch
    asm volatile("pushl %0\n"
                  "pushl %1\n"
                  "pushfl  \n"
                  "pushl %2\n"
                  "pushl %3\n"
                  "iret\n"
		         :
                 :"g"(iret_ds), "g" (iret_esp), "g" (iret_cs), "g" (iret_eip)
		         );

    return 0;
}

//halt
/* 
 * halt
 *   DESCRIPTION: halts the current user program
 *   INPUTS: status - allows us to return different execute values
 *   OUTPUTS: returns value to execute
 *   RETURN VALUE: whether the execute worked properly
 *   SIDE EFFECTS: halts another user program
 */

int32_t halt(uint8_t status){

    // close relevant fds
    int i; 
    for (i = 2; i < 8; i++) {
        close(i);
    }

    uint32_t ebp_to = curr_PCBs[current_task]->exec_ebp; //get execute ebp
    uint32_t return_value_eax = 0; //get value to return to execute
    processes_map = processes_map ^ (1<<curr_pids[current_task]); //xor current process, which is 1, to make it to 0
    if(curr_pids[current_task] < 3){
        isBaseTask = 1;
        execute((const uint8_t*)"shell"); //if current shell, run again
    }

    //restore parent data to the global variables
    curr_pids[current_task] = curr_PCBs[current_task]->prev_pid;
    curr_esp0s[current_task] = curr_PCBs[current_task]->prev_esp0;
    curr_ss0s[current_task] = curr_PCBs[current_task]->prev_ss0;
    if(curr_PCBs[current_task]->isVidMapSet == 1){
        unset_vidmap_memory();
        curr_PCBs[current_task]->isVidMapSet = 0;
    }

    curr_PCBs[current_task] = (PCB_begin*) (curr_PCBs[current_task]->PCB_begin_prev);
    
    
    if(status == 1)
        return_value_eax = 256; //if coming from exception, execute will return proper value

    
    


    set_paging_for_user_process(curr_pids[current_task]); //change paging to parent data

    

    tss.ss0 = curr_ss0s[current_task];
    tss.esp0 = curr_esp0s[current_task]; //set proper tss values
    
    asm volatile ("movl %0, %%eax\n"
                  "movl %1, %%esp\n"
                  "popl %%ebp\n"
                  "ret\n"
                    :
                    :"g"(return_value_eax), "g" (ebp_to)
                    :"eax", "esp", "ebp"); //jump to execute and return

    return -1;
}


/* 
 * write
 *   DESCRIPTION: calls the a appropriate write function depending on fd file type
 *   INPUTS: int32_t fd, const void* buf, int32_t nbytes
 *   OUTPUTS: 0 on success; -1 on Failure
 *   RETURN VALUE: whether the write worked properly
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    //cli();
    //check bad values
    if (fd < 0 ||fd >= 8 || buf == NULL || nbytes < 0) {
        printf("SYS WRITE:Invalid Params\n");
        //sti();
        return -1;
    }

    file_descriptor_entry cur_fd = curr_PCBs[current_task]->file_descriptor_array[fd]; //get the file descriptor

    //check if the fd is inactive
    if (cur_fd.flags == 0) {
        printf("SYS WRITE:Inactive FD\n");
        //sti();
        return -1;
    }
    
    //else call & return the correct write
    //sti();
    return cur_fd.file_operations_table->gen_write(fd, buf, nbytes);
}


/* 
 * read
 *   DESCRIPTION: calls the a appropriate read function depending on fd file type
 *   INPUTS: int32_t fd, void* buf, int32_t nbytes
 *   OUTPUTS: 0 on success; -1 on Failure
 *   RETURN VALUE: whether the read worked properly
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    //check bad values
    if (fd < 0 || fd >= 8 || buf == NULL || nbytes < 0) {
        printf("SYS READ:Invalid Params\n");
        return -1;
    }

    file_descriptor_entry cur_fd = curr_PCBs[current_task]->file_descriptor_array[fd]; //get the file descriptor

    //check if the fd is inactive
    if (cur_fd.flags == 0) {
        printf("SYS READ:Inactive FD\n");
        return -1;
    }

    //else call & return the correct write
    return cur_fd.file_operations_table->gen_read(fd, buf, nbytes);
}


/* 
 * open
 *   DESCRIPTION: calls the a appropriate open function depending on file name
 *   INPUTS: const uint8_t* filename
 *   OUTPUTS: 0 on success; -1 on Failure
 *   RETURN VALUE: whether the open worked properly
 */
int32_t open(const uint8_t* filename)
{
    // get the file dentry
    dentry_t new_dentry;
    dir_open(filename); //set the global offset for dir to 0
    if (read_dentry_by_name(filename, &new_dentry) == -1) {
        return -1;
    }

    // loop through FDA for an open spot
    int i; 
    for(i = 0; i < 8; i++)
    {
        //if there is an open spot set the file descriptor entries accordingly
        if ((curr_PCBs[current_task]->file_descriptor_array[i].flags == 0)){

            curr_PCBs[current_task]->file_descriptor_array[i].inode = new_dentry.inode_num;
            curr_PCBs[current_task]->file_descriptor_array[i].flags = 1;
            curr_PCBs[current_task]->file_descriptor_array[i].file_position = 0;
            curr_PCBs[current_task]->file_descriptor_array[i].file_operations_table = get_fod(&new_dentry);

            return i;
        }
    }

    //else array is full return -1
    return -1;
}

/* 
 * close
 *   DESCRIPTION: calls the a appropriate close function depending on fd
 *   INPUTS: int32_t fd
 *   OUTPUTS: 0 on success; -1 on Failure
 *   RETURN VALUE: whether the close worked properly
 */
int32_t close(int32_t fd) {
    //bad val check
    if (fd <=1 || fd >= 8) {
        return -1;
    }
    //check if fd is still open
    if ((curr_PCBs[current_task]->file_descriptor_array[fd].flags != 0)){
        curr_PCBs[current_task]->file_descriptor_array[fd].file_operations_table->gen_close(fd); //call close

        //if fd isnt stdin or stdout set flags to 0
        if(fd > 1)
            curr_PCBs[current_task]->file_descriptor_array[fd].flags = 0;
        return 0;
    }
       
    //fd already closed
    return -1;

}



/* 
 * get_fod
 *   DESCRIPTION: returns the correct funtion pointer table based on file type
 *   INPUTS: dentry_t* file
 *   OUTPUTS: pointer to function pointer table
 *   RETURN VALUE: pointer to function pointer table
 */
fops_t* get_fod(dentry_t* file) 
{
    //check bad pointer
    if (file == NULL) {
        return NULL;
    }

    int32_t ftype = file->filetype; //get the file type

    //check bad ftypes
    if(ftype < 0 || ftype > 2) {
        return NULL;
    }

    //if the ftype is 0 it is rtc
    if (ftype == 0) {
        return &rtc_table;
    }

    //if the ftype is 1 it is dir file
    if (ftype == 1) {
        return &fdir;
    }

    //if the ftype is 2 it is file
    if (ftype == 2) {
        return &ffil;
    }

    //else return NULL
    return NULL;

}


/* 
 * init_fda
 *   DESCRIPTION: inits all the fda positions to val
 *   INPUTS: PCB_begin* pcb, file_descriptor_entry val
 *   OUTPUTS: uniform fda
 */
void init_fda(PCB_begin* pcb, file_descriptor_entry val) {
    pcb->file_descriptor_array[0] = val;
    pcb->file_descriptor_array[1] = val;
    pcb->file_descriptor_array[2] = val;
    pcb->file_descriptor_array[3] = val;
    pcb->file_descriptor_array[4] = val;
    pcb->file_descriptor_array[5] = val;
    pcb->file_descriptor_array[6] = val;
    pcb->file_descriptor_array[7] = val;
}

/* 
 * getargs
 *   DESCRIPTION: puts the side arguments of call into buffer
 *   INPUTS: buf* -> pointer to char array, int32_t nbytes -> number of bytes to copy
 *   OUTPUTS: returns 0 on success, -1 on failure
 *   RETURN VALUE: whether the getargs worked properly
 *   SIDE EFFECTS: allows user program to have access to sideargs
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
    if(buf == NULL || nbytes < 0 || (char*)curr_PCBs[current_task]->side_args == '\0')
        return -1;
    if(strlen((void*)curr_PCBs[current_task]->side_args) == 0) return -1;
    strncpy((int8_t*) buf, (int8_t*) curr_PCBs[current_task]->side_args, nbytes); //copy bytes into buf, dont have to worry about overflow, taken care of by function
    return 0;
}

/* 
 * vidmap
 *   DESCRIPTION: puts the pointer of the pointer to screen_start to mapped video memory
 *   INPUTS: uint8_t** screen_start
 *   OUTPUTS: returns 0 on success, -1 on failure
 *   RETURN VALUE: whether the vidmap worked properly
 *   SIDE EFFECTS: allows user program to have access to video memory
 */
int32_t vidmap (uint8_t** screen_start){
    if(screen_start == NULL)
        return -1;

    if((uint32_t)screen_start < 0x8000000 ||  (uint32_t)screen_start >= 0x8400000) //check if screen_start is in userspace
        return -1;

    if(curr_PCBs[current_task]->isVidMapSet == 0){
        set_vidmap_memory();
        change_vidmap_memory(current_task);
        curr_PCBs[current_task]->isVidMapSet = 1; //set map to video memory
    }
    
    uint32_t ans = 0x8400000 | VIDEO_MAP[current_task];

    *screen_start = (uint8_t*) ans; //set screen start

    return 0;
    
}
