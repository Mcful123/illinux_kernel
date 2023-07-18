// #include "tests.h"
// #include "x86_desc.h"
// #include "lib.h"
// #include "handlers.h"
// #include "terminal.h"

// #include "file_system.h"

// #define PASS 1
// #define FAIL 0

// /* format these macros as you see fit */
// #define TEST_HEADER 	
// 	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
// #define TEST_OUTPUT(name, result)	
// 	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

// static inline void assertion_failure(){
// 	/* Use exception #15 for assertions, otherwise
// 	   reserved by Intel */
// 	asm volatile("int $15");
// }


// /* Checkpoint 1 tests */

// /* IDT Test - Example
//  * 
//  * Asserts that first 10 IDT entries are not NULL
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  * Coverage: Load IDT, IDT definition
//  * Files: x86_desc.h/S
//  */
// int idt_test(){
// 	TEST_HEADER;

// 	int i;
// 	int result = PASS;
// 	for (i = 0; i < 10; ++i){
// 		if ((idt[i].offset_15_00 == NULL) && 
// 			(idt[i].offset_31_16 == NULL)){
// 			assertion_failure();
// 			result = FAIL;
// 		}
// 	}

// 	return result;
// }

// // add more tests here

// int syscall_test(){
// 	TEST_HEADER;
// 	int result = PASS;
	
	
// 	asm volatile("int $0x80");
// 	/*
// 	assertion_failure();
// 	result = FAIL;
// 	*/	

// 	return result;
// }

// int exception_test(){
// 	TEST_HEADER;
// 	int result = PASS;
// 	int zer = 0;
// 	int g = 12/zer;
// 	g++;
	
	
	
// 	/*
// 	assertion_failure();
// 	result = FAIL;
// 	*/	

// 	return result;
// }


// int rtc_test(){
// 	clear();
// 	TEST_HEADER;
// 	int result = PASS;
// 	printf("\nThis is 2 Hz:    ");
// 	rtc_write(2);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 4 Hz:    ");
// 	rtc_write(4);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 8 Hz:    ");
// 	rtc_write(8);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 16 Hz:   ");
// 	rtc_write(16);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 32 Hz:   ");
// 	rtc_write(32);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 64 Hz:   ");
// 	rtc_write(64);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 128 Hz:  ");
// 	rtc_write(128);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 256 Hz:  ");
// 	rtc_write(256);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 512 Hz:  ");
// 	rtc_write(512);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\nThis is 1024 Hz: ");
// 	rtc_write(1024);
// 	while(rtc_counter < 20){};
// 		rtc_counter = 0;

// 	printf("\n");
//     rtc_open();

// 	rtc_close();
// 	disable_irq(0x08);

// 	/*
// 	assertion_failure();
// 	result = FAIL;
// 	*/	

// 	return result;
// }

// /* Page Fault Test
//  * 
//  * Calls page fault if trying to access uninitiated memory location
//  * Inputs: None
//  * Outputs: FAIL / calls page fault
//  * Side Effects: stuck in page fault
//  * Coverage: page fault / page initialization
//  * Files: paging.c/.h
//  */
// int pagefault_test(){
// 	TEST_HEADER;

// 	int* test = (int*)(0x900000);
// 	int poop = *test;
	
// 	poop = 4;
// 	return FAIL;
// }


// /* Page initialized Test
//  * 
//  * Check if pages are iniitialized
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: None
//  * Coverage: page initialization
//  * Files: paging.c/.h
//  */
// int pageinit_test(){
// 	TEST_HEADER;

// 	int* test = (int*)(0xB8005);
// 	int poop = *test;

// 	poop = 4;
// 	return PASS;
// }
// /* Checkpoint 2 tests */
// int terminal_io_test(){
// 	TEST_HEADER;
// 	uint8_t buffer_test[128];
// 	int32_t actual_size;
// 	while(1){
// 		printf("Enter your name: ");
// 		actual_size = terminal_read(buffer_test, 128);
// 		printf("\n Hi ");
// 		terminal_write(buffer_test, actual_size);
// 		printf("What's up? \n");
// 	}
	
// 	return PASS;
// }




// /* Checkpoint 2 tests */
// int keyboard_test(){
// 	TEST_HEADER;
// 	uint8_t buffer_test[128];
// 	int32_t actual_size;
// 	while(1){
// 		actual_size = terminal_read(buffer_test, 128);
// 		printf("This is what is in the terminal buffer btw: \n");
// 		terminal_write(buffer_test, actual_size);
// 	}
// 	return PASS;
// }



// /* show fish Test
//  * 
//  * Check if filesystem works 
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: prints content of file onto screen
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int show_fish ()
// {
// 	TEST_HEADER;
// 	clear();
//     int32_t fd, i, poop;
//     uint8_t buf[9999];

// 	poop = f_open ((uint8_t*)"frame0.txt");	
// 	if(poop != 0)
// 		return FAIL;
// 	poop = f_read (fd, (int8_t*)buf, 9999);
// 	if(poop <= 0)
// 		return FAIL;
// 	for(i=0; i<poop; i++){
// 		putc(buf[i]);
// 	}
// 	return PASS;
// }

// /* read exe Test
//  * 
//  * Check if filesystem works 
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: prints content of file onto screen
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int read_exe ()
// {
// 	TEST_HEADER;
// 	clear();
//     int32_t fd, i, poop;
//     uint8_t buf[9999];

// 	poop = f_open ((uint8_t*)"hello");	
// 	if(poop != 0){
// 		return FAIL;
// 	}
// 	poop = f_read (fd, (int8_t*)buf, 9999);
// 	if(poop <= 0){
// 		return FAIL;
// 	}
// 	for(i=0; i<poop; i++){
// 		putc(buf[i]);
// 	}
// 	printf("\n");
// 	return PASS;
// }

// /* read long Test
//  * 
//  * Check if filesystem works 
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: prints content of file onto screen
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int read_long ()
// {
// 	TEST_HEADER;
// 	clear();
//     int32_t fd, i, poop;
//     uint8_t buf[9999];

// 	poop = f_open ((uint8_t*)"verylargetextwithverylongname.tx");	
// 	printf("flag\n");
// 	if(poop != 0){
// 		return FAIL;
// 	}
// 	poop = f_read (fd, (int8_t*)buf, 9999);
// 	if(poop <= 0){
// 		return FAIL;
// 	}
// 	for(i=0; i<poop; i++){
// 		putc(buf[i]);

// 	}
// 	return PASS;
// }

// /* ls Test
//  * 
//  * Check if filesystem works 
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: prints file names and their file type
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int ls_test(){
// 	TEST_HEADER;
// 	clear();
// 	int32_t poop, fd, i;
// 	uint8_t buf[100];
// 	poop = dir_open((uint8_t*)".");
// 	if(poop != 0)
// 		return FAIL;
// 	while(1){
// 		poop = dir_read(fd, (int8_t*)buf, 100);
// 		if(poop < 0){
// 			break; 
// 		}
// 		printf("filename: ");
// 		for(i=0; i<40; i++){
// 			if(i < poop-1)
// 				putc(buf[i]);
// 			else
// 				printf(" ");
// 		}
// 		printf("type: %d\n", buf[poop]);
// 	}
// 	return PASS;
// }

// /* bad_name_test
//  * 
//  * Check putting in invalid name to read
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: none
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int bad_name_test(){
// 	TEST_HEADER;
// 	int8_t poop;
// 	poop = f_open ((uint8_t*)"randomename_ahhh");
// 	if(poop == -1)
// 		return PASS;	
// 	else
// 		return FAIL;
// }

// /* NULL buffer test
//  * 
//  * Check putting in null buffer 
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: none
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int null_buffer_test(){
// 	TEST_HEADER;
//     int32_t fd, poop;
//     int8_t* buf = NULL;

// 	poop = f_open ((uint8_t*)"frame0.txt");	
// 	if(poop != 0){
// 		return FAIL;
// 	}
// 	poop = f_read (fd, buf, 9999);
// 	if(poop <= 0){
// 		return PASS;
// 	}
	
// 	return FAIL;
// }

// /* file / dir close and write test
//  * 
//  * Check putting in null buffer 
//  * Inputs: None
//  * Outputs: PASS / FAIL
//  * Side Effects: none
//  * Coverage: file system
//  * Files: file_system.c/.h
//  */
// int write_close_test(){
// 	TEST_HEADER;
//     int32_t poop;
// 	poop = dir_write();
// 	if(poop != -1) // write should always fail
// 		return FAIL;
// 	poop = dir_close(); // close should always pass
// 	if(poop != 0)
// 		return FAIL;
// 	poop = f_write(); // wrote should always fail
// 	if(poop != -1)
// 		return FAIL;
// 	poop = f_close(); // close should always pass
// 	if(poop != 0)
// 		return FAIL;
// 	return PASS;
// }
// /* Checkpoint 3 tests */
// /* Checkpoint 4 tests */
// /* Checkpoint 5 tests */


// /* Test suite entry point */
// void launch_tests(){
// 	disable_irq(0x08);
// 	TEST_OUTPUT("idt_test", idt_test());
// 	// TEST_OUTPUT("system call test", syscall_test());
// 	// TEST_OUTPUT("page init test", pageinit_test());
// 	// TEST_OUTPUT("terminal driver test", terminal_io_test());
// 	// TEST_OUTPUT("page fault test", pagefault_test());
// 	// TEST_OUTPUT("page init test", pageinit_test());
// 	// TEST_OUTPUT("exception_test", exception_test());
// 	// TEST_OUTPUT("page init test", pageinit_test());
	
// 	// TEST_OUTPUT("keyboard test", keyboard_test());

	
// 	/* _________these are the test cases for file systems. only uncomment one at a time________ */
// 	TEST_OUTPUT("ls test", ls_test());
// 	// TEST_OUTPUT("show fish", show_fish());
// 	// TEST_OUTPUT("reading hello executable", read_exe());
// 	// TEST_OUTPUT("reading very long thing", read_long());
// 	// TEST_OUTPUT("Bad name test", bad_name_test()); // the next two can stay uncommented always
// 	// TEST_OUTPUT("null buffer test", null_buffer_test());
// 	// TEST_OUTPUT("writes and closes test", write_close_test());

// 	// TEST_OUTPUT("page fault test", pagefault_test());
// 	// TEST_OUTPUT("exception_test", exception_test());
// 	// TEST_OUTPUT("rtc_test", rtc_test());
// 	// launch your tests here
// }
