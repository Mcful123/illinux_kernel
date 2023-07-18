#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "handlers.h"
#include "system_calls.h"
#include "terminal_switch.h"
/****************************************EXCEPTION HANDLER***********************************/
char* error_messages[20] = {
    " i can't divide by zero \n",
    " debug bro \n",
    " this is a non maskable interrupt lmao get rekt \n",
    " breakpoint alert \n",
    " arrgghh i am overflowing \n",
    " bound range error \n",
    " that's an invalid opcode \n",
    " coprocessor error no devices \n",
    " double fault \n",
    " co processor segment overrun \n",
    " invalid tss \n",
    " segment not present \n",
    " stack segment fault \n",
    " general protection fault YOU SHALL NOT PASS\n",
    " page fault \n",
    " reserved by intel shush \n",
    " floating point exception \n",
    " alignment check \n",
    " machine check \n",
    " simd floating point error \n"
};

/* 
 * exception_handler
 *   DESCRIPTION: displays the error message and halts the user program
 *   INPUTS: id -- refers to which exception has occured 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void exception_handler(uint32_t id){
    printf(error_messages[id]);
    // asm volatile("hlt");
    // while(1){}
    sti();
    halt(1);

    return;
}
void division_error(){
    // intel defined division error is 0
    exception_handler(0);
    return;
}
void debug(){
    // intel defined debug is 1
    exception_handler(1);
    return;
}
void nmi(){
    // intel defined nmi is 2
    exception_handler(2);
    return;
}
void breakpoint(){
    // intel defined breakpoint is 3
    exception_handler(3);
    return;
}
void overflow(){
    // intel defined overflow is 4
    exception_handler(4);
    return;
}
void bound_range_exceeded(){
    // intel defined bound_range_exceeded is 5
    exception_handler(5);
    return;
}
void invalid_opcode(){
    // intel defined invalid_opcode is 6
    exception_handler(6);
    return;
}
void coprocessor_not_avail(){
    // intel defined coprocessor_not_avail is 7
    exception_handler(7);
    return;
}
void double_fault(){
    // intel defined double_fault is 8
    exception_handler(8);
    return;
}
void coprocessor_segment_overrun(){
    // intel defined coprocessor_segment_overrun is 9
    exception_handler(9);
    return;
}
void invalid_tss(){
    // intel defined invalid_tss is 10
    exception_handler(10);
    return;
}
void segment_not_present(){
    // intel defined segment_not_present is 11
    exception_handler(11);
    return;
}
void stack_seg_fault(){
    // intel defined stack_seg_fault is 12
    exception_handler(12);
    return;
}
void gen_protection(){
    // intel defined gen_protection is 13
    exception_handler(13);
    return;
}
void page_fault(){
    // intel defined page_fault is 14
    exception_handler(14);
    return;
}
void reserved(){
    // intel defined reserved is 15
    exception_handler(15);
    return;
}
void floating_point_err(){
    // intel defined floating_point_err is 16
    exception_handler(16);
    return;
}
void align_check(){
    // intel defined align_check is 17
    exception_handler(17);
    return;
}
void machine_check(){
    // intel defined machine_check is 18
    exception_handler(18);
    return;
}
void simd_floating_point_exc(){
    // intel defined simd_floating_point_exc is 19
    exception_handler(19);
    return;
}

/****************************************PIT HANDLER*****************************************/
/* 
 * init_pit
 *   DESCRIPTION: initializes the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void init_pit() {
    int hz = 2000;
    outb(0x36, 0x43);
    outb((1193180/hz) & 0xFF, 0x40);
    outb((1193180/hz) >> 8, 0x40);
    enable_irq(PIC_PIT_PORT);
}

/* 
 * pit_handler
 *   DESCRIPTION: initializes the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_handler(){
    
    if(curr_PCBs[0] == NULL){
        send_eoi(PIC_PIT_PORT);
        isBaseTask = 1;
        execute((const uint8_t*)"shell");
        return;
    }
    int temp = (current_task+1)%3;
    
    switch_to_task(temp);
    send_eoi(PIC_PIT_PORT);
    
}



/****************************************RTC HANDLER*****************************************/
volatile int rtc_interrupt_occurred[3] = {0, 0, 0};
int rtc_freq[3] = {1024, 1024, 1024};
volatile int rtc_counter[3] = {0, 0, 0};
/* 
 * init_rtc
 *   DESCRIPTION: initializes the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void init_rtc(){
    outb(REGISTER_B, RTC_PORT);
    char prev = inb(CMOS_PORT);
    outb(REGISTER_B, RTC_PORT);
    outb(prev | 0x40, CMOS_PORT);
    enable_irq(0x08);
    // rtc_counter = 0;
}

/* 
 * rtc_handler
 *   DESCRIPTION: this is the RTC handler which gets called when the RTC interrupts
 *   INPUTS: none 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void rtc_handler(){
    send_eoi(PIC_RTC_PORT);
    // rtc_counter++;
    //printf("#");
    // test_interrupts();
    int i = 0;
    for(i=0; i <3; i++) {
        rtc_counter[i]++;
        rtc_counter[i]%=rtc_freq[i];
        if(rtc_counter[i] == 0){
            rtc_interrupt_occurred[i] = 1;
        }
    }
    // rtc_interrupt_occurred[0] = 1;
    // rtc_interrupt_occurred[1] = 1;
    // rtc_interrupt_occurred[2] = 1;
    outb(REGISTER_C, RTC_PORT);
    inb(CMOS_PORT);
}

/* 
 * rtc_open
 *   DESCRIPTION:
 *   INPUTS: const uint8_t* filename 
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_open(const uint8_t* filename){
    // 0x0f refers to 1024Hz frequency of the RTC
    char rate = 0x06;
    /* set the RTC freq to 1024HZ */
    outb(REGISTER_A, RTC_PORT);		// set index to register A, disable NMI
    char prev=inb(CMOS_PORT);	// get initial value of register A
    outb(REGISTER_A, RTC_PORT);		// reset index to A
    // flag with 0xf0 to get the top 4 bits
    outb((prev & 0xF0) | rate, CMOS_PORT); //write only our rate to A.
    return 0;
}

/* 
 * rtc_read - check if an interrupt occured otherise keep looping
 *   DESCRIPTION:
 *   INPUTS: int32_t fd, void* buf, int32_t nbytes 
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
     // if the onterrupt hasn't occured stay in loop
    while(!rtc_interrupt_occurred[current_task]){

    }
    // reset flag
    rtc_interrupt_occurred[current_task] = 0;
    return 0;
}

/* 
 * rtc_write - function to chnage the frequency of the rtc
 *   DESCRIPTION:
 *   INPUTS: int32_t fd, const void* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on fail; nbytes on success
 *   SIDE EFFECTS: none
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    // char rate = 0x00;
    if(buf == NULL || nbytes != 4) return -1;
    int frequency = *(int *)(buf);

    // if(frequency != 2 && frequency != )
    rtc_freq[current_task] = 1024/frequency;
    rtc_counter[current_task] = 0;


    // Reference: https://courses.engr.illinois.edu/ece391/fa2021/secure/references/ds12887.pdf
    // switch(frequency){
    //     case 2:     rate = 0x0f; break; // 0x0f means 2Hz
    //     case 4:     rate = 0x0e; break; // 0x0e means 4Hz
    //     case 8:     rate = 0x0d; break; // 0x0d means 8Hz
    //     case 16:    rate = 0x0c; break; // 0x0c means 16Hz
    //     case 32:    rate = 0x0b; break; // 0x0b means 32Hz
    //     case 64:    rate = 0x0a; break; // 0x0a means 64Hz
    //     case 128:   rate = 0x09; break; // 0x09 means 128Hz
    //     case 256:   rate = 0x08; break; // 0x08 means 256Hz
    //     case 512:   rate = 0x07; break; // 0x07 means 512Hz
    //     case 1024:  rate = 0x06; break; // 0x06 means 1024Hz
    //     default: return -1;
    // }
    // outb(REGISTER_A, RTC_PORT);		// set index to register A, disable NMI
    // char prev=inb(CMOS_PORT);	// get initial value of register A
    // outb(REGISTER_A, RTC_PORT);		// reset index to A
    // outb((prev & 0xF0) | rate, CMOS_PORT); //write only our rate to A.
    return nbytes;
}


/* 
 * rtc_close - close the rtc (no special opertion)
 *   DESCRIPTION:
 *   INPUTS: int32_t fd 
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(int32_t fd){
    return 0;
}


/************************************HANDLER DISPATCHER**************************************/
/* 
 * do_irq
 *   DESCRIPTION: this is a dispatcher function which calls the correct handler depending on 
 *                the exception and interrupt
 *   INPUTS: irq_regs --  look at struct definitions in handlers.h
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void do_irq(irq_handler_t irq_regs){
    unsigned long int_num = irq_regs.irq_number;
    common_handler_table[int_num]();
}

/*********************************SYSTEM CALL DISPATCHER**************************************/
/* 
 * system_call
 *   DESCRIPTION: this is the RTC handler which gets called when the RTC interrupts
 *   INPUTS: none 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void system_call(){
    printf("System call was invoked.\n");   //print statement in replace for actual system call
}
