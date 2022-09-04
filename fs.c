#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fs.h"


/* 
struct superblock
{
    // we could also have size of inodes
    int num_inodes;
    int num_blocks;
    int size_blocks;
};

struct inode{
    int size;
    char name[8];
};

struct disk_block
{
    int next_block_num;
    char data[512];
};

*/

struct superblock sb;
struct inode *inodes;       // inodes array
struct disk_block *dbs;     // disk blocks array


// initialize new file system
void 
create_fs()
{
    sb.num_inodes  = 10;
    sb.num_blocks  = 100;
    sb.size_blocks = sizeof(struct disk_block);


    int i;
    inodes = (struct inode*)malloc(sizeof(struct inode) * sb.num_inodes);
    for(i=0; i < sb.num_inodes; i++){
        *(inodes + i) = (struct inode){
            .size           = -1,
            .first_block    = -1,
            .name           = "emptyfi"
        };
    } // init inodes


    dbs = (struct disk_block*)malloc(sizeof(struct disk_block) * sb.num_blocks);
    for(i=0; i < sb.num_blocks; i++){
        *(dbs + i) = (struct disk_block){
            .next_block_num = -1,
        };
    } // init db

} // create_fs

// load a file system
// and copy/write data into it
void 
mount_fs()
{
    FILE *file;
    file = fopen("fs_data", "r");

    fread(&sb, sizeof(struct superblock), 1, file);

    inodes = (struct inode*)malloc(sizeof(struct inode) * sb.num_inodes);
    fread(inodes, sizeof(struct inode), sb.num_inodes, file);

    dbs = (struct disk_block*)malloc(sizeof(struct disk_block) * sb.num_blocks);
    fread(dbs, sizeof(struct disk_block), sb.num_blocks, file);

    fclose(file);

} // mount_fs

// write the file system
void 
sync_fs()
{
    FILE *file;
    file = fopen("fs_data", "w+");

    // superblock
    fwrite(&sb, sizeof(struct superblock), 1, file);

    fwrite(inodes, sizeof(struct inode), sb.num_inodes, file);
    fwrite(dbs, sizeof(struct disk_block), sb.num_blocks, file);

    fclose(file);
} // sync_fs     


// print out info about the file system
void 
print_fs()
{
    printf("***Superblock info***\n");
    printf("\tnum inodes : %d\n", sb.num_inodes);
    printf("\tnum blocks : %d\n", sb.num_blocks);
    printf("\tsize blocks: %d\n\n", sb.size_blocks);

    printf("***inodes***\n");
    int i;
    for(i=0; i < sb.num_inodes; i++){
        printf("\tsize: %d, block: %d, name: %s\n", (inodes + i)->size, (inodes + i)->first_block,(inodes + i)->name);
    }
    printf("\n"); 

    printf("***blocks***\n");
    for(i=0; i < sb.num_blocks; i++){
        printf("\tblock num: %d, next block: %d\n", i, (dbs + i)->next_block_num);
    } 
} // print_fs


int 
find_empty_inode()
{
    int i;
    for(i=0; i<sb.num_inodes; i++){
        if((inodes + i)->first_block == -1){
            return i;
        }
    }

    return -1;
} // find_empty_inode


int
find_empty_block()
{
    int i;
    for(i=0; i<sb.num_blocks; i++){
        if((dbs + i)->next_block_num == -1){
            return i;
        }
    }

    return -1;
} // find_empty_block


// allocate file
// return file number
int
allocate_file(char name[8])
{
    // find empty inode and block and claim them
    int in = find_empty_inode();
    int block = find_empty_block();

    (inodes + in)->first_block = block;
    (dbs + block)->next_block_num = -2;

    strcpy((inodes + in)->name, name);

    // return the file descriptor
    return in;    
} // allocate_file

void
shorten_file(int bn)
{
    int nn = (dbs + bn)->next_block_num;
    if(nn >= 0){
        shorten_file(nn);
    }
    (dbs + bn)->next_block_num = -1;
} // shorten file

// add/delete blocks
void
set_filesize(int filenum, int size)
{
    // how many blocks should we have
    int tmp = size + BLOCKSIZE - 1;
    int num = tmp / BLOCKSIZE;

    int bn = (inodes + filenum)->first_block;

    num--;
    // grow the file if neccessary
    while(num > 0){
        // check next block number
        int next_num = (dbs + bn)->next_block_num;
        if(next_num == -2){
            int empty = find_empty_block();
            (dbs + bn)->next_block_num = empty;
            (dbs + empty)->next_block_num = -2;
        }
        bn = (dbs + bn)->next_block_num;
        num--;
    }


    // shorten if neccessary
    shorten_file(bn);
    (dbs + bn)->next_block_num = -2;

} // set_filesize


int
get_block_num(int file, int offset)
{
    int togo = offset;
    int bn = (inodes + file)->first_block;

    while(togo){
        bn = (dbs + bn)->next_block_num;
        togo--;
    }

    return bn;

} // get_block_num

void
write_byte(int filenum, int pos, char *data)
{
    // calculate which block
    int relative_block = pos / BLOCKSIZE;

    // find the block number
    int bn = get_block_num(filenum, relative_block);

    // calculate the offset in the block
    int offset = pos % BLOCKSIZE;
    
    // write the data
    (dbs + bn)->data[offset] = (*data);

} // write byte


