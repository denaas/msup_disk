#pragma once
extern "C"
{
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
}
#include <string>
#include <fstream>
#include <iostream>
//#include <sys/stat.h>


using namespace std;

struct attr
{
    char s[20];
    int in;
};

class Storage
{
    public:
    Storage(const char* path="./HelloWorld.txt");
    ~Storage();
    void AddFile(const char* path, const char* name);
    void LoadFile(const char* name, const char* dest);
    int FindFile(const char* name);
    void ClearStorage(void);
    
    private:
    int fd;
    int block_size;//4KB
    int file_size;//1GB, in blocks
    int i_arr_size;//Number of inodes
    int i_free;//Number of free inodes
    int b_free;//Number of free blocks
    unsigned char i_arr[255];//Inode array
    int b_arr[100];// Free blocks array
    char* s_path;//Path to storage
    int inode[12];//10 direct addresses, 1 sub addr, 1 file size
    int i_blocks;
    void update_sblock(void);
    void read_sblock(void);
    void add_free_block(int n);
    int take_free_block(void);
    void add_free_inode(int n);
    int take_free_inode(void);
};
