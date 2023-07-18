#include "types.h"



//Struct to represent the Page Directory Entries 
typedef union pdesc_entry_t {
    //table_ent_t* pg_table;             //if the pd == 0 then this page table should point to a 
                                
        unsigned long val;
        struct{
            //32-bit directory entry
            unsigned long p         :1;   //1 if present 0 = not present 
            unsigned long rw        :1;   //always 1
            unsigned long usr_spr   :1;   //1 for user level | 0 for kernel 
            unsigned long pwt       :1;   //always 0
            unsigned long pcd       :1;   // 1 for program code & data, 0 for video mem
            unsigned long a         :1;   //Accessed| not used set to 0
            unsigned long d         :1;   //dirty set to 0 per descriptors.pdf
            unsigned long ps        :1;   //Page size| 1 for 4MB page, 0 for 4kb page table
            unsigned long g         :1;   //Global bit set 1 for kernel, 0 else
            unsigned long avl       :3;   // always 000
            // if the entry is 4MB, the following 20 bits is a 20 bit address
            // of the entry is a 4kb, the following 20 bits is broken up to its appropriate sections
            unsigned long pat       :1;   
            unsigned long adr_39    :4;   // bits 39-32 of address
            unsigned long res       :5;   // reserved
            unsigned long addr      :10;  //upper 10-bit address
        } __attribute__ ((packed));
    
} desc_ent_t;

typedef union ptentry_t {
    //unsigned long whole_entry;    //whole 32 bit value per documentation
        unsigned long val;
        struct{
            //used for table and 4MB page entries
            unsigned long p          :1;   //1 if present 0 = not present
            unsigned long rw         :1;   //always 1
            unsigned long usr_spr    :1;   //1 for user level | 0 for kernel 
            unsigned long pwt        :1;   //always 0
            unsigned long pcd        :1;   // 1 for program code & data, 0 for video mem
            unsigned long a          :1;   //Accessed| not used set to 0
            unsigned long d          :1;   //dirty set to 0 per descriptors.pdf
            unsigned long pat        :1;   //always 0
            unsigned long g          :1;   //Global bit set 1 for kernel, 0 else
            unsigned long avl        :3;   // 3 bit for avail. always0 000
            unsigned long addr       :20;  //20-bit address
        } __attribute__ ((packed));
   

} table_ent_t; 


//called in kernel.c to initialize paging
void page_init();

//Used to FLush the TLB 
void flush_TLB();

//initializes a page descriptor entry struct to initial values
void initPDentry(desc_ent_t* entry);

//initializes a page table entry struct to initial values
void initPTentry(table_ent_t* entry);

//initializes the desciptor and page table
void initTables(desc_ent_t* pdt, table_ent_t* pt);
// sets the mapping of 128 to 132 MB based on process_number
void set_paging_for_user_process(uint32_t process_number);
// maps part of the virtual memory accessible to the user program to the video memory
void set_vidmap_memory();
//sets the user part of video memory to not-present, so it cant be used
void unset_vidmap_memory();

void change_vidmap_memory(uint32_t task);

void move_video_memory(uint32_t terminal_num);
