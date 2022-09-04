// we'll use a file allocation table approach

#define BLOCKSIZE 512

// meta info about file system e.g
// number of inodes
// number of disk blocks
// size of disk blocks

struct superblock
{
    // we could also have size of inodes
    int num_inodes;
    int num_blocks;
    int size_blocks;
};


struct inode{
    int size;
    int first_block;
    char name[8];
};

struct disk_block
{
    int next_block_num;
    char data[BLOCKSIZE];
};


void create_fs();                   // initialize new file system
void mount_fs();                    // load a file system
void sync_fs();                     // write the file system

// return file number
int allocate_file(char name[8]);    
void set_filesize(int filenum, int size);
void write_byte(int filename, int pos, char *data);

// todo
// 1. delete
// 2. read byte
// 3. and many more...

void print_fs();                    // print out info about the file system



