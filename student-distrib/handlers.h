#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#ifndef _HANDLERS_H
#define _HANDLERS_H


/****************************************EXCEPTION HANDLER***********************************/
// functions to call the exception_handler with the correct id
void division_error();
void debug();
void nmi();
void breakpoint();
void overflow();
void bound_range_exceeded();
void invalid_opcode();
void coprocessor_not_avail();
void double_fault();
void coprocessor_segment_overrun();
void invalid_tss();
void segment_not_present();
void stack_seg_fault();
void gen_protection();
void page_fault();
void reserved();
void floating_point_err();
void align_check();
void machine_check();
void simd_floating_point_exc();
// function to print the error message and halt user program 
void exception_handler(uint32_t id);

/****************************************PIT HANDLER*****************************************/

#define PIC_PIT_PORT 0x00
// initiliazes the rtc by sendinf commands to register B 
void init_pit();
// reads register C and get the time
void pit_handler();



/****************************************RTC HANDLER*****************************************/
#define RTC_PORT 0x70               // RTC_PORT is connected to 0x70 on the system
#define CMOS_PORT 0x71
#define REGISTER_A 0x8A
#define REGISTER_B 0x8B
#define REGISTER_C 0x8C
#define PIC_RTC_PORT 0x08
// function used to test whetehr yhe rtc interrupts multiple times (deprecated)

// initiliazes the rtc by sendinf commands to register B 
void init_rtc();
// reads register C and get the time
void rtc_handler();
// open the rtc 
int32_t rtc_open(const uint8_t* filename);
// close the rtc
int32_t rtc_close(int32_t fd);
// read from the rtc
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
// change the frequency of the rtc and write
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

/*****************************************HANDLER ASM*****************************************/
// function which pushed the registers and calls the do_irq dispatcher
extern void common_exception();
// array pointer which jumps to the common_exception after pushing the id 
extern void *common_exception_table[20];
// function which pushes the registers and calls the do_irq dispatcher
extern void common_interrupt();
// array pointer which jumps to the common_interrupt after pushing the id 
extern void *common_interrupt_table[20];
// function to push the registers before calling the kernel from user code
extern void common_syscall();

/***********************************HANDLER DISPATCHER****************************************/
// struct with all the registers and id number
typedef struct {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    //uint32_t xds;
    //uint32_t xes;
    uint32_t irq_number;
} irq_handler_t;
// function pointer array with all the handlers (exception and hardware)
void (*common_handler_table[NUM_VEC])();
// dispatcher fucntion whichc alls the common_handler_table
void do_irq(irq_handler_t irq_regs);


/*********************************SYSTEM CALL DISPATCHER**************************************/
// dummy system_call fucntion to show asm linkage
void system_call();

#endif /* _HANDLERS_H */
