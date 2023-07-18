#include "file_system.h"
#include "lib.h"
#include "system_calls.h"
#include "terminal_switch.h"

/* global variables */
bootb_t* boot;
uint32_t g_offset;
dentry_t temp; 
dentry_t* g_dentry = &test;
uint32_t fs_offset;

/*_________________________________Driver Functions__________________________________________*/
/*
 * f_read
 *   DESCRIPTION: reads a file
 *   INPUTS: int32_t fd, void* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success and -1 for fail
 *   SIDE EFFECTS: none
 */ 
int32_t f_read(int32_t fd, void* buf, int32_t nbytes) 
{
    // if(buf == NULL)
    //     return -1;
    // return read_data(g_dentry->inode_num, g_offset, (uint8_t*)buf, (uint32_t)nbytes);
    // if(buf == NULL)
    //     return -1;

    uint32_t inode = curr_PCBs[current_task]->file_descriptor_array[fd].inode;
    uint32_t file_position = curr_PCBs[current_task]->file_descriptor_array[fd].file_position;
    
    //TO DO: check file_position validity 
    uint32_t offset = read_data(inode, file_position, (void*)buf, (uint32_t)nbytes);
    curr_PCBs[current_task]->file_descriptor_array[fd].file_position += offset;
    return offset;
}
/*
 * f_write
 *   DESCRIPTION: does nothing
 *   INPUTS: int32_t fd, const void* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: always -1 for fail for read-only system
 *   SIDE EFFECTS: none
 */
int32_t f_write(int32_t fd, const void* buf, int32_t nbytes) 
{
    return -1;
}
/*
 * f_open
 *   DESCRIPTION: opens a file to do things with
 *   INPUTS: const uint8_t* filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, -1 for fail
 *   SIDE EFFECTS: loads dentry to the global variable
 */ 
int32_t f_open(const uint8_t* filename) 
{
    // initialize the data block offset to 0
    g_offset = 0;
    return read_dentry_by_name(filename, g_dentry);
}
/*
 * f_close
 *   DESCRIPTION: undoes the thing done by f_open
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */ 
int32_t f_close(int32_t fd) 
{
    return 0;
}

/* directory functions */
/*
 * dir_read
 *   DESCRIPTION: reads directory file contents
 *   INPUTS: int32_t fd, void* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: the number of bytes read into the buffer
 *   SIDE EFFECTS: none
 */ 
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
    int8_t* temp_buf = (int8_t*) buf;
    // null checking
    if(buf == NULL)
        return -1;
    fs_offset = curr_PCBs[current_task]->fs_offset;
    
    uint8_t i;
    int ret = read_dentry_by_index(fs_offset, g_dentry);
    // check to see if reading by index was successful
    if(ret != 0)
        return 0;

    // get the file name and file type
    int8_t* name = g_dentry->filename;
    int32_t name_len = (int32_t)strlen((int8_t*)name);

    // write to the buffer
    if(name_len > 32) name_len = 32; // 32 is the max length of file name
    for(i=0; i<name_len; i++){ // hardcode buf size to 40 for this checkpoint
        temp_buf[i] = name[i];
    }
    curr_PCBs[current_task]->fs_offset++; // increment for next file to read
    return name_len;
} 
/*
 * dir_open
 *   DESCRIPTION: opens a directory 
 *   INPUTS: const uint8_t* dname
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success and -1 for fail
 *   SIDE EFFECTS: loads the global dentry variable
 */ 
int32_t dir_open(const uint8_t* dname) 
{
    //initialize the file offset to zero 
    // fs_offset = 0;
    return 0;
}

/*
 * dir_close
 *   DESCRIPTION: undoes the thing done by dir_open
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: none
 */ 
int32_t dir_close(int32_t fd) 
{
    return 0;
}
/*
 * dir_write
 *   DESCRIPTION: does nothing
 *   INPUTS: int32_t fd, const void* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: always -1 for fail for read-only system
 *   SIDE EFFECTS: none
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) 
{
    return -1;
}
/*_________________________________Module Functions__________________________________________*/
/*
 * read_dentry_byb_name
 *   DESCRIPTION: finds the file and loads its dentry
 *   INPUTS: const uint8_t* fname, dentry_t* dentry
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success and -1 for fail
 *   SIDE EFFECTS: changes the dentry passed in 
 */ 
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
    int i;
    if(strlen((int8_t*)fname) > 32)
        return -1;
    for(i=0; i<boot->dir_count; i++){ // loop through the dentries
        uint32_t flen = strlen((int8_t*)(boot->dentries[i].filename)) > strlen((int8_t*)(fname)) ? strlen((int8_t*)(boot->dentries[i].filename)):strlen((int8_t*)(fname)); 
        flen = 32;
        if(strncmp((int8_t*)fname, (int8_t*)boot->dentries[i].filename, flen) == 0){ // check if names are equal
            // copy elements 
            strcpy(dentry->filename, boot->dentries[i].filename);
            dentry->filetype = boot->dentries[i].filetype;
            dentry->inode_num = boot->dentries[i].inode_num;
            return 0;
        }
    }
    return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: finds the file by its index and load its dentrty
 *   INPUTS: uint32_t index, dentry_t* dentry
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success and -1 for fail
 *   SIDE EFFECTS: changes the dentry passed in 
 */ 
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
    // boundary checks
    if(dentry == NULL || index < 0 || index >= boot->dir_count) 
    {
        return -1;
    }

    // copy elements
    strcpy(dentry->filename, boot->dentries[index].filename);
    dentry->filetype = boot->dentries[index].filetype;
    dentry->inode_num = boot->dentries[index].inode_num;
    
    return 0;
}

/*
 * read_data
 *   DESCRIPTION: read the contents of a file
 *   INPUTS: uint32_t inode, uint32_t offset, void* buf, uint32_t length
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes read. -1 for fail
 *   SIDE EFFECTS: none
 */ 
int32_t read_data (uint32_t inode, uint32_t offset, void* buf, uint32_t length)
{
    //NULL check
    if (buf == NULL)
        return -1;
    int8_t* temp_buf = (int8_t*) buf;

    //get the current inode
    inode_t* cur_inode = (inode_t*)(boot + inode+1);

    //if the offset is the length of the file then return 0 (we already read the file)
    if (offset == cur_inode->length){
        return 0;
    }
    //if the offset is greater, then it's an invalid read. return -1.
    if (offset > cur_inode->length){
        return -1;
    }

    //Inode count(N) and Datablock count(D)
    uint32_t N = boot->inode_count;
    uint32_t D = boot->data_count;

    //Check if Inode number is in range
    if (inode < 0 || inode > N-1) {
        return -1;
    }

    //check if all the data block values are valid before copy anything to buffer
    if (!(valid_db(cur_inode, D))) 
    {
        return -1;
    }
   
    // calculate offsets for byte-level and datablock-level
    int32_t block_offset = offset / FOURKB;
    int32_t byte_offset = offset % FOURKB;
    
    // if the bytes to read are bigger than the bytes left in file, change it so it can't read more 
    uint32_t left_over = cur_inode->length - offset; 
    if (left_over < length) {
        length = left_over;
    }

    // looping through file and reading byte by byte to the buffer
    int32_t byte_count = 0;
    int i;
    for(i=0; i<length; i++){

        uint32_t block_num = i/FOURKB;
        int32_t d_block_idx = cur_inode->data_block_num[block_offset + block_num];
        uint8_t* data = (uint8_t*)((uint32_t)boot + FOURKB*(N+d_block_idx+1) + byte_offset + byte_count);
        
        temp_buf[i] = *data;
        byte_count++;
        
        // check if at end of data block and then change pointers to top of data block
        if(FOURKB - byte_count == byte_offset){
            byte_offset = 0;
            byte_count = 0;
        }
    }
    
    return length;

}

/*___________________________________Helper Functions_______________________________*/
/*
 * valid_db
 *   DESCRIPTION: checks if the data block is a valid one
 *   INPUTS: inode_t* inode, uint32_t max
 *   OUTPUTS: none
 *   RETURN VALUE: 1 for valid, 0 for not
 *   SIDE EFFECTS: none
 */ 
uint32_t valid_db(inode_t* inode, uint32_t max)
{
    uint32_t i = 0;                     //looping index
    uint32_t len = inode->length;       //get length of file
    uint32_t lim = len/FOURKB;          //calculate the number of data blocks in the inode
    lim++;
    
    //loop over the data blocks 
    for (i = 0; i < lim; i++) 
    {
        //if the data block number is greater then the max number then invalid
        if (inode->data_block_num[i] > max) 
        {
            return 0;
        }
    }
    return 1;
}


