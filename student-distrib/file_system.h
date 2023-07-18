#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "types.h"

#define NAME_LEN 32
#define DENTRY_LIM 63
#define BBRES 52
#define DENTRES 24
#define FOURKB 4096

/*   structs needed for the file systems   */
// dentry struct
typedef struct dentry_t dentry_t;
struct __attribute__((__packed__))dentry_t {
    int8_t filename[NAME_LEN];
    int32_t filetype;
    int32_t inode_num; 
    int8_t reserved[DENTRES];
};

// boot block struct
typedef struct bootb_t bootb_t;
struct __attribute__((__packed__))bootb_t {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[BBRES];
    dentry_t dentries[DENTRY_LIM];
};

// inode struct
typedef struct inode_t inode_t;
struct __attribute__((__packed__)) inode_t {
    int32_t length;
    int32_t data_block_num[1023];
};

/* global variables */
extern bootb_t* boot;
uint32_t g_offset; 
dentry_t test; 
dentry_t* g_dentry;
uint32_t fs_offset;

/*  file functions declarations  */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, void* buf, uint32_t length);
int32_t f_read(int32_t fd, void* buf, int32_t nbytes);
int32_t f_open(const uint8_t* filename);
int32_t f_close(int32_t fd);
int32_t f_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_open(const uint8_t* dname);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_close(int32_t fd);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
uint32_t valid_db(inode_t* inode, uint32_t max);


#endif 
