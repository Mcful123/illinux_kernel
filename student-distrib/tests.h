#ifndef TESTS_H
#define TESTS_H

// test launcher
void launch_tests();
int syscall_test();
int pagefault_test();
int pageinit_test();
int exception_test();
int idt_test();
int rtc_test();
int terminal_io_test();
int ls_test ();
int show_fish();
int read_exe();
int read_long();
#endif /* TESTS_H */
