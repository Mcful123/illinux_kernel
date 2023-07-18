#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"

//function to initialize idt in kernel.c
void initialize_idt(idt_desc_t* idt);

#endif /* _IDT_H */
