#define ASM     1

.text
.globl common_exception, common_exception_table     # function to handle exceptions
.globl common_interrupt, common_interrupt_table     # function to handle interrupts
.globl common_syscall, common_syscall_table         # functions to handle system calls




common_syscall:
    sti         # clear interrupts
    cmpl $1, %eax
    jl fail_common_syscall
    cmpl $10, %eax      # check for proper input
    jg fail_common_syscall
    pushl %eax
    addl $-1, %eax  # get proper index into jump table
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx      # push all registers for linkage
    call *common_syscall_table(, %eax, 4)
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    addl $4, %esp
    jmp return_from_common_syscall
fail_common_syscall:    
    movl $-1, %eax
return_from_common_syscall:    
    iret            # return since it is from interrupt handler


common_syscall_table:   #system call table
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    # .long set_handler
    # .long sigreturn


common_interrupt:   #assembly linkage for all interrupt handlers
    cli             #disable interrupts
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx      #push all registers
    call do_irq
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax       # pop all registers
    addl $4, %esp       # add 4 to stack because of irq number
    sti             # enable interrupts
    iret


common_interrupt_table: #interrupt table for current support interrupts
    .long rtc       #goes to rtc handler
    .long keyboard  #goes to keyboard handler
    .long pit

rtc:
    pushl $0x28             # push x28 because irq8 corresponds to x28 in the idt
    jmp common_interrupt    # go to complete assembly linkage

keyboard:
    pushl $0x21             # push x21 because irq1 corresponds to x21 in idt
    jmp common_interrupt    # go to complete assembly linkage

pit:
    pushl $0x20             # push x21 because irq0 corresponds to x20 in idt
    jmp common_interrupt    # go to complete assembly linkage






common_exception:
    cli     # clear interrupts
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx  #push registers
    call do_irq #perform irq function
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    addl $4, %esp   #pop all registers and add 4 for the irq number
    sti     #enable interrupts
    iret

common_exception_table: #all the first 20 intel defined exceptions
    .long division_error
    .long debug
    .long nmi
    .long breakpoint
    .long overflow
    .long bound_range_exceeded
    .long invalid_opcode
    .long coprocessor_not_avail
    .long double_fault
    .long coprocessor_segment_overrun
    .long invalid_tss
    .long segment_not_present
    .long stack_seg_fault
    .long gen_protection
    .long page_fault
    .long reserved
    .long floating_point_err
    .long align_check
    .long machine_check
    .long simd_floating_point_exc

division_error:
    pushl $0    #the idt entry for the exception
    jmp common_exception
debug:
    pushl $1    #the idt entry for the exception
    jmp common_exception
nmi:
    pushl $2    #the idt entry for the exception
    jmp common_exception
breakpoint:
    pushl $3    #the idt entry for the exception
    jmp common_exception
overflow:
    pushl $4    #the idt entry for the exception
    jmp common_exception
bound_range_exceeded:
    pushl $5    #the idt entry for the exception
    jmp common_exception
invalid_opcode:
    pushl $6    #the idt entry for the exception
    jmp common_exception
coprocessor_not_avail:
    pushl $7    #the idt entry for the exception
    jmp common_exception
double_fault:
    pushl $8    #the idt entry for the exception
    jmp common_exception
coprocessor_segment_overrun:
    pushl $9    #the idt entry for the exception
    jmp common_exception
invalid_tss:
    pushl $10   #the idt entry for the exception
    jmp common_exception
segment_not_present:
    pushl $11   #the idt entry for the exception
    jmp common_exception
stack_seg_fault:
    pushl $12   #the idt entry for the exception
    jmp common_exception
gen_protection:
    pushl $13   #the idt entry for the exception
    jmp common_exception
page_fault:
    pushl $14   #the idt entry for the exception
    jmp common_exception
reserved:
    pushl $15   #the idt entry for the exception
    jmp common_exception
floating_point_err:
    pushl $16   #the idt entry for the exception
    jmp common_exception
align_check:
    pushl $17   #the idt entry for the exception
    jmp common_exception
machine_check:
    pushl $18   #the idt entry for the exception
    jmp common_exception
simd_floating_point_exc:
    pushl $19   #the idt entry for the exception
    jmp common_exception
