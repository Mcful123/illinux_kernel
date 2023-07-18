#include "types.h"
#include "file_system.h"
#ifndef X_SYSTEM_CALLS_H
#define X_SYSTEM_CALLS_H

#define FDALEN 8


//system call for halting a program
int32_t halt(uint8_t status);
//system call for executing a program
int32_t execute(const uint8_t* command);
//system call for reading from a certain file
int32_t read(int32_t fd, void* buf, int32_t nbytes);
//system call to write to a certain file
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
//system call to open a file
int32_t open(const uint8_t* filename);
//system call to close a file
int32_t close(int32_t fd);
//system call to get args of an argument
int32_t getargs(uint8_t* buf, int32_t nbytes);
//system call to do vidmap
int32_t vidmap(uint8_t** screen_start);
//system call - idk what it does 
int32_t set_handler(int32_t signum, void* handler_address);
//system call - idk what it does
int32_t sigreturn(void);


// extern uint32_t processes_map;
// extern uint32_t curr_pid;
// extern uint32_t curr_esp0;
// extern uint16_t curr_ss0;
// extern PCB_begin* curr_PCB;



//stdin and out function table structs
typedef struct fops_t fops_t;
struct fops_t{
    int32_t (*gen_open)(const uint8_t* filename);
    int32_t (*gen_close)(int32_t fd);
    int32_t (*gen_read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*gen_write)(int32_t fd, const void* buf, int32_t nbytes);
};

// File Descriptor
typedef struct {
    struct fops_t* file_operations_table;     // function pointer table
    uint32_t inode;                                 // inode index
    uint32_t file_position;                         // file offset
    uint32_t flags;                                 // FD in use flag
} file_descriptor_entry;

// PCB struct
// typedef struct PCB_begin PCB_begin;
//struct that represents the PCB
typedef struct {
    file_descriptor_entry file_descriptor_array[FDALEN];     // FD array
    uint32_t prev_pid;                                  // Parent PID
    uint32_t prev_esp0;                                 // Parent stack pointer
    uint32_t prev_ss0;                                  // Parent Segment Selector
    struct PCB_begin* PCB_begin_prev;                   // Parent PCB
    uint32_t exec_ebp;
    uint32_t fs_offset;
    uint8_t side_args[128];
    uint8_t isVidMapSet;
    // uint32_t curr_esp;
    // uint32_t curr_ebp;
} PCB_begin ;


uint32_t processes_map; //global variable of bitmap which determines which processes are executing

// uint32_t curr_pid; //global variable of current pid
// uint32_t curr_esp0; //global variable of current esp0
// uint16_t curr_ss0;  //global variable of current ss0
// PCB_begin* curr_PCB;    //global variable for the pointer of the current PCB

uint32_t curr_pids[3];
uint32_t curr_esp0s[3];
uint32_t curr_esps[3];
uint32_t curr_ebps[3];
uint16_t curr_ss0s[3];
PCB_begin* curr_PCBs[3];

uint32_t isBaseTask;

// helper functions 
fops_t* get_fod(dentry_t* file);
void init_fda(PCB_begin* pcb, file_descriptor_entry val);

#endif /* X_SYSTEM_CALLS_H */
