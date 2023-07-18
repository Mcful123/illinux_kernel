#include "lib.h"
#include "idt.h"
#include "multiboot.h"
#include "i8259.h"
#include "handlers.h"
#include "keyboard.h"

/* void initialize_idt(idt_desc_t* idt)
 * Inputs: idt_desc_t* idt
 * Return Value: void
 * Function: Fills in values for the idt point by idt_desc_t* idt */

void initialize_idt(idt_desc_t* idt){
        int i;

        common_handler_table[0] = division_error; //interrupt handler for int 0x0
        common_handler_table[1] = debug; //interrupt handler for int 0x1
        common_handler_table[2] = nmi; //interrupt handler for int 0x2
        common_handler_table[3] = breakpoint; //interrupt handler for int 0x3
        common_handler_table[4] = overflow; //interrupt handler for int 0x4
        common_handler_table[5] = bound_range_exceeded; //interrupt handler for int 0x5
        common_handler_table[6] = invalid_opcode;   //interrupt handler for int 0x6
        common_handler_table[7] = coprocessor_not_avail;    //interrupt handler for int 0x7
        common_handler_table[8] = double_fault; //interrupt handler for int 0x8
        common_handler_table[9] = coprocessor_segment_overrun;  //interrupt handler for int 0x9
        common_handler_table[10] = invalid_tss; //interrupt handler for int 0xA
        common_handler_table[11] = segment_not_present; //interrupt handler for int 0xB
        common_handler_table[12] = stack_seg_fault; //interrupt handler for int 0xC
        common_handler_table[13] = gen_protection;  //interrupt handler for int 0xD
        common_handler_table[14] = page_fault;  //interrupt handler for int 0xE
        common_handler_table[15] = reserved;    //interrupt handler for int 0xF
        common_handler_table[16] = floating_point_err;  //interrupt handler for int 0x10
        common_handler_table[17] = align_check; //interrupt handler for int 0x11
        common_handler_table[18] = machine_check;   //interrupt handler for int 0x12
        common_handler_table[19] = simd_floating_point_exc; //interrupt handler for int 0x13


        for(i=0; i < 20; i++) {     //allow for first 20 exceptions in idt
            idt[i].present      = 0x01; //set present to 1 for exceptions
            idt[i].seg_selector = KERNEL_CS;    //goes through kernel
            idt[i].reserved4    = 0x0;  
            idt[i].reserved3    = 0x0;
            idt[i].reserved2    = 0x1;
            idt[i].reserved1    = 0x1;
            idt[i].size         = 0x1;
            idt[i].reserved0    = 0x0;  //these values are defined according to descriptors.pdf
            idt[i].dpl = 0x0;          //exceptions called only by kernel
            SET_IDT_ENTRY(idt[i], common_exception_table[i]);   //set every excpetion to assembly linkage for exceptions
        }

        //0x28 because IRQ8 is related to x28 on the idt
        common_handler_table[0x28] = rtc_handler; // function to test rtc interrupts
        idt[0x28].present      = 0x1;   //set present to 1
        idt[0x28].seg_selector = KERNEL_CS; //goes through kernel
        idt[0x28].reserved4    = 0x0;
        idt[0x28].reserved3    = 0x0;
        idt[0x28].reserved2    = 0x1;
        idt[0x28].reserved1    = 0x1;
        idt[0x28].size         = 0x1;
        idt[0x28].reserved0    = 0x0;   //these values are defined according to descriptors.pdf
        idt[0x28].dpl = 0x0; //only called by kernel
        SET_IDT_ENTRY(idt[0x28], common_interrupt_table[0]); //first entry in assembly linkage of interrupt table

        //0x20 because IRQ0 is related to x20 on the idt
        common_handler_table[0x20] = pit_handler; // function to test rtc interrupts
        idt[0x20].present      = 0x1;   //set present to 1
        idt[0x20].seg_selector = KERNEL_CS; //goes through kernel
        idt[0x20].reserved4    = 0x0;
        idt[0x20].reserved3    = 0x0;
        idt[0x20].reserved2    = 0x1;
        idt[0x20].reserved1    = 0x1;
        idt[0x20].size         = 0x1;
        idt[0x20].reserved0    = 0x0;   //these values are defined according to descriptors.pdf
        idt[0x20].dpl = 0x0; //only called by kernel
        SET_IDT_ENTRY(idt[0x20], common_interrupt_table[2]); //first entry in assembly linkage of interrupt table


        //0x21 because IRQ1 is related to x21 on the idt
        common_handler_table[0x21] = keyboard_handler; // function to handle keyboard interrupts
        idt[0x21].present      = 0x1;   //set present to 1
        idt[0x21].seg_selector = KERNEL_CS; //goes through kernel
        idt[0x21].reserved4    = 0x0;
        idt[0x21].reserved3    = 0x0;
        idt[0x21].reserved2    = 0x1;
        idt[0x21].reserved1    = 0x1;
        idt[0x21].size         = 0x1;
        idt[0x21].reserved0    = 0x0;   //these values are defined according to descriptors.pdf
        idt[0x21].dpl = 0x0; //only called by kernel
        SET_IDT_ENTRY(idt[0x21], common_interrupt_table[1]);    //second entry in assembly linkage of interrupt table

        //0x80 because system calls are entry x80 on idt
        idt[0x80].present      = 0x1;   //set present to 1
        idt[0x80].seg_selector = KERNEL_CS; //goes through kernel
        idt[0x80].reserved4    = 0x0;
        idt[0x80].reserved3    = 0x0;
        idt[0x80].reserved2    = 0x1;
        idt[0x80].reserved1    = 0x1;
        idt[0x80].size         = 0x1;
        idt[0x80].reserved0    = 0x0;   //these values are defined according to descriptors.pdf
        idt[0x80].dpl = 0x3;    //called by userspace and kernel
        SET_IDT_ENTRY(idt[0x80], common_syscall);   //link to assembly linkage for system call

}
