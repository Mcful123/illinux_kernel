/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/* void i8259_init(void)
 * Inputs: void
 * Return Value: void
 * Function: initializes the master and the slave pic to recieve interrupts */

void i8259_init(void) {

    master_mask = 0xff; //all interrupts should be masked
    slave_mask = 0xff;  //all interrupts should be masked

    /* mask all the inputs */
    outb(master_mask, MASTER_PORT + 0x01);  //send mask to master pic data port, so add 1
    outb(slave_mask, SLAVE_PORT + 0x01); // send mask to slave data port, so add 1


    /* Initialize master PIC */
    outb(ICW1,MASTER_PORT); 
    outb(ICW1, SLAVE_PORT); //send command word to slave pic

    outb(ICW2_MASTER, MASTER_PORT + 0x01);  //send command word 2 to master data port
    outb(ICW2_SLAVE, SLAVE_PORT + 0x01);    //send command word 2 to slave data port

    outb(ICW3_MASTER, MASTER_PORT + 0x01);  //send command word 3 to master data port
    outb(ICW3_SLAVE, SLAVE_PORT + 0x01);    //send command word 3 to slave data port
    
    outb(ICW4, MASTER_PORT + 0x01);     //send command word 4 to master data port
    outb(ICW4, SLAVE_PORT + 0x01);  //send command word 4 to slave data port


    /* Initialize slave PIC */
    
    outb(master_mask, MASTER_PORT + 0x01);  //mask again to prevent all interrupts on master pic
    outb(slave_mask, SLAVE_PORT + 0x01);    //mask again to prevent all interrupts on slave pic
    
    enable_irq(0x02);   //enable interrupts on IRQ2, because this is slave pic
    //still no interrupts can occur because slave pic is still masked

}

/* Enable (unmask) the specified IRQ */
/* void enable_irq
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Function: Enable irq based on a certain number*/
void enable_irq(uint32_t irq_num) {

    if(irq_num > 15)
        return;  //if irq number is invalid, do nothing. IRQs are numbered from 0-15
    if(irq_num <8) {    //irq 0-7 is only on master pic
        master_mask = inb(MASTER_PORT + 0x01);  //get current mask on master pic
        master_mask = master_mask & (~(1 << irq_num));  //change mask on master pic
        outb(master_mask, MASTER_PORT + 0x01);  //apply mask on master pic
    }
    else{
        irq_num -=8;    //since on slave pic, need to subtract by 8
        slave_mask = inb(SLAVE_PORT + 0x01);    //get current mask on slave pic
        slave_mask = slave_mask & (~(1 << irq_num));    //change mask on slave pic
        outb(slave_mask, SLAVE_PORT + 0x01);    //apply mask on slave pic
    }
    return;
}

/* Disable (mask) the specified IRQ */
/* void disable_irq
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Function: Disable irq based on a certain number*/
void disable_irq(uint32_t irq_num) {
    if(irq_num > 15)
        return;  //since IRQs are from 0-15, greater than 15 is bogus
    if(irq_num <8) {    //only master pic has IRQ 0-7
        master_mask = inb(MASTER_PORT + 0x01);  //get current mask from master pic
        master_mask = master_mask | (1 << irq_num); //set mask based on irq number
        outb(master_mask, MASTER_PORT + 0x01);  //set mask on master pic
    }
    else{
        irq_num -=8;    //if irq > 8, need to subtract so we can properly bit shift
        slave_mask = inb(SLAVE_PORT + 0x01);    //get current mask from slave port
        slave_mask = slave_mask | (1 << irq_num);   //set mask
        outb(slave_mask, SLAVE_PORT + 0x01); //apply mask on slave pic
    }
    return;
}

/* Send end-of-interrupt signal for the specified IRQ */
/* void send_eoi
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Function: Send eoi to pic based on irq_num*/
void send_eoi(uint32_t irq_num) {
    if(irq_num > 15)
        return;  //since IRQs are from 0-15, greater than 15 is bogus
    if(irq_num >= 8){ // if irq_num is greater than 8, go send to slave and master pic

        outb((uint8_t) EOI | (irq_num-8), SLAVE_PORT);  //subtract 8 to be correct number on slabve pic
        outb((uint8_t) EOI | 2, MASTER_PORT);   //need to send EOI to master since IRQ2 master is slave pic, so 2 is needed
    }
    else{
        outb((uint8_t) EOI | irq_num, MASTER_PORT); //just send eoi to master pic else

    }

    return;

}
