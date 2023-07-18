#include "paging.h" 
#include "lib.h"          //for video page memory location
#include "terminal_switch.h"

#define KILO 1024           // Number of entries in the pg direc & pg table
#define FOURKB 4096         // alignment of mem
#define INIT_VAL 0x00000002 // initialize r/w ???
#define PAGE_DIRECTORY 69   // temporary. fill this
#define VIDEO  0xB8000      //video location
#define FOURMB 0x400000     //four megabyte value
#define VIDEO_TASK1 0xB9000
#define VIDEO_TASK2 0xBA000
#define VIDEO_TASK3 0xBB000
#define VIDEO_MAP1 0xBC000
#define VIDEO_MAP2 0xBD000
#define VIDEO_MAP3 0xBE000

static uint32_t VIDEO_TASK[3] = {VIDEO_TASK1, VIDEO_TASK2, VIDEO_TASK3};
static uint32_t VIDEO_MAP[3] = {VIDEO_MAP1, VIDEO_MAP2, VIDEO_MAP3};

// creating page table and page directory

static desc_ent_t page_direc[KILO] __attribute__((aligned(FOURKB)));    //Page directory for all 4GB mem
static table_ent_t page_table[KILO] __attribute__((aligned(FOURKB)));   //page table for first 4MB of mem
unsigned long first_idx = ((VIDEO & 0xFFF00000)>>22);                   //index into pde
unsigned long second_idx = ((VIDEO & 0x003FF000)>>12);                  //index into page table
unsigned long third_idx = (VIDEO & 0x00000FFF);                         //page offset

/*
 * page_init
 *   DESCRIPTION: Initaliazes the proper tables for paging
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes paging
 */   

void page_init(){

    //set all the values in the tables
    initTables(page_direc, page_table);
    
    //set first value of page directory to page table
    unsigned long temp_addr = (unsigned long)page_table & 0xFFFFF000; // get top 20 bits
    //page_direc[first_idx].pg_table = page_table;
    page_direc[first_idx].addr = temp_addr >> 22;                   //top 10 bits of addr
    page_direc[first_idx].res = (temp_addr & 0x003E0000) >> 17;     // bits 21 - 17 of address
    page_direc[first_idx].adr_39 = (temp_addr & 0x0001E000) >> 13;  //bits 16 - 13 of address
    page_direc[first_idx].pat = (temp_addr & 0x00001000) >> 12;     // bit 12 of address
    page_direc[first_idx].g = 0;            //0 for user 
    page_direc[first_idx].ps = 0;           //points to page table
    page_direc[first_idx].a = 0;            //always 0
    page_direc[first_idx].avl = 0;          //always 0 
    page_direc[first_idx].pcd = 1;          // 1 for program code & data, 0 for video mem
    page_direc[first_idx].pwt = 0;          //always 0
    page_direc[first_idx].usr_spr = 1;      //1 for user level | 0 for kernel 
    page_direc[first_idx].rw = 1;           //always 1
    page_direc[first_idx].p = 1;            //1 for present

    //set kernel value
    
    page_direc[1].addr = ((FOURMB & 0xFFF00000)>>22);     //top 10 bits of addr
    page_direc[1].g = 1;            //1 for kernel
    page_direc[1].ps = 1;           //4mb page
    page_direc[1].a = 0;            //always 0
    page_direc[1].pcd = 1;          // 1 for program code
    page_direc[1].pwt = 0;          //always 0
    page_direc[1].avl = 0;          //always 0
    page_direc[1].usr_spr = 0;      //1 for user level | 0 for kernel 
    page_direc[1].rw = 1;           //always 1
    page_direc[1].p = 1;            //1 for present
    page_direc[1].pat = 0;          //always 0
    page_direc[1].d = 0;            //always 0
    page_direc[1].res = 0;          //always 0           
 
    //set video page value

    //used for table and 4MB page entries
    page_table[second_idx].addr = (VIDEO & 0xFFFFF000) >> 12;        //20-bit address
    page_table[second_idx].g = 0;            //0 for user
    page_table[second_idx].a = 0;            //always 0
    page_table[second_idx].pcd = 0;          // 0 for video memory
    page_table[second_idx].pwt = 0;          //always 0
    page_table[second_idx].usr_spr = 1;      //1 for user level
    page_table[second_idx].rw = 1;           //always 1
    page_table[second_idx].p = 1;            //1 for present
    page_table[second_idx].pat = 0;          //always 0
    page_table[second_idx].d = 0;            //always 0

    uint32_t temp_idx = ((VIDEO_TASK1 & 0x003FF000)>>12);

    page_table[temp_idx].addr = (VIDEO_TASK1 & 0xFFFFF000) >> 12;        //20-bit address
    page_table[temp_idx].g = 0;            //0 for user
    page_table[temp_idx].a = 0;            //always 0
    page_table[temp_idx].pcd = 0;          // 0 for video memory
    page_table[temp_idx].pwt = 0;          //always 0
    page_table[temp_idx].usr_spr = 1;      //1 for user level
    page_table[temp_idx].rw = 1;           //always 1
    page_table[temp_idx].p = 1;            //1 for present
    page_table[temp_idx].pat = 0;          //always 0
    page_table[temp_idx].d = 0;            //always 0

    temp_idx = ((VIDEO_TASK2 & 0x003FF000)>>12);

    page_table[temp_idx].addr = (VIDEO_TASK2 & 0xFFFFF000) >> 12;        //20-bit address
    page_table[temp_idx].g = 0;            //0 for user
    page_table[temp_idx].a = 0;            //always 0
    page_table[temp_idx].pcd = 0;          // 0 for video memory
    page_table[temp_idx].pwt = 0;          //always 0
    page_table[temp_idx].usr_spr = 1;      //1 for user level
    page_table[temp_idx].rw = 1;           //always 1
    page_table[temp_idx].p = 1;            //1 for present
    page_table[temp_idx].pat = 0;          //always 0
    page_table[temp_idx].d = 0;            //always 0

    temp_idx = ((VIDEO_TASK3 & 0x003FF000)>>12);

    page_table[temp_idx].addr = (VIDEO_TASK3 & 0xFFFFF000) >> 12;        //20-bit address
    page_table[temp_idx].g = 0;            //0 for user
    page_table[temp_idx].a = 0;            //always 0
    page_table[temp_idx].pcd = 0;          // 0 for video memory
    page_table[temp_idx].pwt = 0;          //always 0
    page_table[temp_idx].usr_spr = 1;      //1 for user level
    page_table[temp_idx].rw = 1;           //always 1
    page_table[temp_idx].p = 1;            //1 for present
    page_table[temp_idx].pat = 0;          //always 0
    page_table[temp_idx].d = 0;            //always 0
    
    //assembly to enable paging (cr3) and enable 4MB pages (cr4)
    asm(
    "\
    movl %%cr4, %%ebx;          \
    orl $0x00000010, %%ebx;     \
    movl %%ebx, %%cr4;          \
    movl %0, %%ebx;             \
    movl %%ebx, %%cr3;          \
    movl %%cr0, %%ebx;          \
    orl $0x80000001, %%ebx;     \
    movl %%ebx, %%cr0;"
    :              
    :"r"(page_direc) //input
    :"%ebx" 
    );

}

/*
 * Flush_TLB 
 *   DESCRIPTION: FLushes TLB by reloading cr3
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: flushes tlb
 */   
void flush_TLB(){
    //assembly to flush tlb by resetting cr3
    asm(
        "movl %%cr3, %%eax;     \
        movl %%eax, %%cr3;"
        :
        :
        : "%eax"
        );
}

//_____________________________________Helper Functions____________________________________
/*
 * initPDentry
 *   DESCRIPTION: Initializes page descripitor entry struct
 *   INPUTS: pointer to page descripitor entry struct
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets all values of struct
 */   
void initPDentry(desc_ent_t* entry) {
    if(entry == NULL){
        return;
    }    
    //sets all values to zero but enables read and write 
    entry->addr = 0x0;
    entry->g = 0x0;
    entry->ps = 0x0;
    entry->a = 0x0;
    entry->pcd = 0x0;
    entry->pwt = 0x0;
    entry->usr_spr = 0x0;
    entry->rw = 0x1;
    entry->p = 0x0;
    entry->pat = 0x0;
    entry->d = 0x0;
    entry->res = 0x0;
    //entry->whole_entry = 0x2;
}

/*
 * page_init
 *   DESCRIPTION: Initializes page table entry struct
 *   INPUTS: pointer to page table entry struct
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets all values of a page table entry struct
 */   
void initPTentry(table_ent_t* entry) {
    if(entry == NULL){
        return;
    }
    //sets all values to zero but enables read and write 
    entry->addr = 0x0;
    entry->g = 0x0;
    entry->a = 0x0;
    entry->pcd = 0x0;
    entry->pwt = 0x0;
    entry->usr_spr = 0x0;
    entry->rw = 0x1;
    entry->p = 0x0;
    entry->pat = 0x0;
    entry->d = 0x0;
    //entry->whole_entry = 0x2;
}

/*
 * initTables
 *   DESCRIPTION: Initaliazes the page descriptor table and page table 
 *   INPUTS: pointer to page table and pdt
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initia
 */   
void initTables(desc_ent_t* pdt, table_ent_t* pt) {
    int i = 0;          //looping index
    //loop over tables and initialize each entry
    for (i = 0; i < KILO; i++) {
        initPDentry(&pdt[i]);
        initPTentry(&pt[i]);
    }
}


/*
acknowledgments: 
https://wiki.osdev.org/
and given ECE391 resources
*/


/* 
 * set_paging_for_user_process
 *   DESCRIPTION: sets 8MB + process_number to 128MB to 132MB in virtual memory
 *   INPUTS: process_number, the process_id to set the virtual memory of
 *   OUTPUTS: nothing
 *   RETURN VALUE: void
 *   SIDE EFFECTS: changes the mapping of virtual memory 128 to 132 MB
 */

void set_paging_for_user_process(uint32_t process_number){

	//32 because every entry corresponds to 4 MB, and 128MB/4MB = 32
    page_direc[32].addr = 2+process_number;     //top 10 bits of addr
    page_direc[32].g = 0;            //0 for user_process
    page_direc[32].ps = 1;           //4mb page
    page_direc[32].a = 0;            //always 0
    page_direc[32].pcd = 1;          // 1 for program code
    page_direc[32].pwt = 0;          //always 0
    page_direc[32].avl = 0;          //always 0
    page_direc[32].usr_spr = 1;      //1 for user level | 0 for kernel 
    page_direc[32].rw = 1;           //always 1
    page_direc[32].p = 1;            //1 for present
    page_direc[32].pat = 0;          //always 0
    page_direc[32].d = 0;            //always 0
    page_direc[32].res = 0;          //always 0    

    flush_TLB(); //flush TLB after changing page

}

/* 
 * set_vidmap_memory
 *   DESCRIPTION: sets virtual address 132MB  + VIDEO to physical video memory
 *   INPUTS: nothing
 *   OUTPUTS: nothing
 *   RETURN VALUE: void
 *   SIDE EFFECTS: allows user program to modify memory
 */
void set_vidmap_memory(){

	unsigned long temp_addr = (unsigned long)page_table & 0xFFFFF000; // get top 20 bits
    page_direc[33].addr = temp_addr >> 22;                   //top 10 bits of addr
    page_direc[33].res = (temp_addr & 0x003E0000) >> 17;     // bits 21 - 17 of address
    page_direc[33].adr_39 = (temp_addr & 0x0001E000) >> 13;  //bits 16 - 13 of address
    page_direc[33].pat = (temp_addr & 0x00001000) >> 12;     // bit 12 of address
    page_direc[33].g = 0;            //0 for user 
    page_direc[33].ps = 0;           //points to page table
    page_direc[33].a = 0;            //always 0
    page_direc[33].avl = 0;          //always 0 
    page_direc[33].pcd = 0;          // 1 for program code & data, 0 for video mem
    page_direc[33].pwt = 0;          //always 0
    page_direc[33].usr_spr = 1;      //1 for user level | 0 for kernel 
    page_direc[33].rw = 1;           //always 1
    page_direc[33].p = 1;            //1 for present
}

/* 
 * change_vidmap_memory
 *   DESCRIPTION: changes vidmap meory
 *   INPUTS: task number
 *   OUTPUTS: nothing
 *   RETURN VALUE: void
 *   SIDE EFFECTS: changes paging of vidmap
 */
void change_vidmap_memory(uint32_t task){
    if(task > 2)
        return;

    uint32_t temp_idx = ((VIDEO_MAP[task] & 0x003FF000)>>12);
    page_table[temp_idx].g = 0;            //0 for user
    page_table[temp_idx].a = 0;            //always 0
    page_table[temp_idx].pcd = 0;          // 0 for video memory
    page_table[temp_idx].pwt = 0;          //always 0
    page_table[temp_idx].usr_spr = 1;      //1 for user level
    page_table[temp_idx].rw = 1;           //always 1
    page_table[temp_idx].p = 1;            //1 for present
    page_table[temp_idx].pat = 0;          //always 0
    page_table[temp_idx].d = 0;            //always 0

    if(task == current_screen){   
        page_table[temp_idx].addr = (VIDEO & 0xFFFFF000) >> 12;        //20-bit address        
    }
    else{
        page_table[temp_idx].addr = (VIDEO_TASK[task] & 0xFFFFF000) >> 12;
    }
    return;
}

/* 
 * unset_vidmap_memory
 *   DESCRIPTION: sets user access to video memory to 0
 *   INPUTS: nothing
 *   OUTPUTS: nothing
 *   RETURN VALUE: void
 *   SIDE EFFECTS: disallows user program to modify memory
 */
void unset_vidmap_memory(){

    page_direc[33].p = 0;            //1 for present

}

/* 
 * move_vidmap_memory
 *   DESCRIPTION: moves videoo memory
 *   INPUTS: nothing
 *   OUTPUTS: nothing
 *   RETURN VALUE: void
 *   SIDE EFFECTS: changes video memory
 */
void move_video_memory(uint32_t terminal_num){
    if(terminal_num > 2 || terminal_num == current_screen)
        return;

    memmove((void*)VIDEO_TASK[current_screen], (void*)VIDEO, 0x1000);
    memmove((void*)VIDEO, (void*)VIDEO_TASK[terminal_num],  0x1000);
    return;
}
