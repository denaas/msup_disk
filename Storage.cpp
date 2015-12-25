#include "Storage.h"

void Storage::ClearStorage(void)
{
    unlink("./info.log");
    unlink(s_path);
    //this->~Storage();
}

int Storage::FindFile(const char* name)
{
    lseek(fd, block_size, SEEK_SET);
    read(fd, inode, sizeof(inode));
    void* buf=calloc(1, block_size);
    lseek(fd, block_size*inode[0], SEEK_SET);
    read(fd, buf, block_size);
    attr* buf1=(attr*)buf;
    for (int i=0; i<block_size/sizeof(attr); i++)
    {
        if (0==strcmp(name,buf1[i].s))
        {
            int n=buf1[i].in;
            free(buf);
            return n;
        }
    }
    free(buf);
    return 0;
}

void Storage::LoadFile(const char* name, const char* dest)
{
    try
    {
        int inum=FindFile(name);
        if (inum==0) throw("No file\n");
        lseek(fd, block_size, SEEK_SET);
        read(fd, inode, sizeof(inode));
        void* buf=calloc(1, block_size);
        lseek(fd, block_size*inode[0], SEEK_SET);
        read(fd, buf, block_size);
        attr* buf1=(attr*)buf;
        for (int i=0; i<block_size/sizeof(attr); i++)
        {
            if (inum==buf1[i].in)
            {
                buf1[i].in=0;
                strcpy(buf1[i].s,"");
                break;
            }
        }
        lseek(fd, block_size*inode[0], SEEK_SET);
        write(fd, buf, block_size);
        free(buf);
        int fdf=creat(dest,S_IRWXU);
        if (fdf==-1) throw("File Creating Error!");
        lseek(fd, block_size + sizeof(inode)*inum, SEEK_SET);
        read(fd, inode, sizeof(inode));
        buf=calloc(1, block_size);
        for (int i=0; i<10&&inode[i]!=0; i++)
        {
            lseek(fd, block_size*inode[i], SEEK_SET);
            read(fd, buf, block_size);
            add_free_block(inode[i]);
            inode[i]=0;
            write(fdf, buf, block_size);
            free(buf);
            buf=calloc(1, block_size);
        }
        if (inode[10]>0)
        {
            int k=block_size/sizeof(int);
            int* buf1=(int*)calloc(k, sizeof(int));
            lseek(fd, block_size*inode[10], SEEK_SET);
            read(fd, buf1, block_size);
            for (int i=0; i<k && buf1[i]!=0; i++)
            {
                lseek(fd, block_size*buf1[i], SEEK_SET);
                read(fd, buf, block_size);
                write(fdf, buf, block_size);
                add_free_block(buf1[i]);
                free(buf);
                buf=calloc(1, block_size);
            }
            add_free_block(inode[10]);
            inode[10]=0;
            free(buf1);
        }
        free(buf);
        lseek(fdf, 0, SEEK_SET);
        ftruncate(fdf,inode[11]);
        add_free_inode(inum);
        
        close(fdf);
    }
    catch(const char* err){cerr<<err<<endl;}

}


void Storage::AddFile(const char* path, const char* name)
{
    try
    {
        int fdf=open(path,O_RDWR);
        if (fdf==-1) throw("File Opening Error!");
        int size=lseek(fdf, 0, SEEK_END);
        for (int i=0; i<12; i++) inode[i]=0;
        inode[11]=size;
        if (size%block_size==0) size=size/block_size; else size=size/block_size+1;
        if (size>b_free) throw("No free space");
        lseek(fdf, 0, SEEK_SET);
        //cerr<<i_free<<endl;
        int inum=take_free_inode();
        //cerr<<i_free<<endl;
        void* buf=calloc(1, block_size);
        for (int i=0; i<10 && size>0; i++, size--)
        {
            inode[i]=take_free_block();
            cerr<<inode[i]<<endl;
            read(fdf, buf, block_size);
            lseek(fd, block_size*inode[i], SEEK_SET);
            write(fd, buf, block_size);
            //cerr<<(char*)buf<<endl;
            free(buf);
            buf=calloc(1, block_size);
        }
        if (size>0)
        {
            int k=block_size/sizeof(int);
            int* buf1=(int*)calloc(k, sizeof(int));
            inode[10]=take_free_block();
            for (int i=0; i<k && size>0; i++, size--)
            {
                buf1[i]=take_free_block();
                read(fdf, buf, block_size);
                lseek(fd, block_size*buf1[i], SEEK_SET);
                write(fd, buf, block_size);
                free(buf);
                buf=calloc(1, block_size);
            }
            lseek(fd, block_size*inode[10], SEEK_SET);
            write(fd, buf1, block_size);
            free(buf1);
        }
        free(buf);
        //
        lseek(fd, block_size + sizeof(inode)*inum, SEEK_SET);
        write(fd, inode, sizeof(inode));
        //
        lseek(fd, block_size, SEEK_SET);
        read(fd, inode, sizeof(inode));
        buf=calloc(1, block_size);
        lseek(fd, block_size*inode[0], SEEK_SET);
        //cerr<<"cat block: "<<inode[0]<<endl;
        read(fd, buf, block_size);
        attr* buf1=(attr*)buf;
        attr a;
        cerr<<sizeof(a)<<endl;
        strcpy(a.s,name);
        a.in=inum;
        while(buf1->in!=0)
        {
            buf1++;
        }
        *buf1=a;
        //strcpy(buf1->s, name);
        lseek(fd, block_size*inode[0], SEEK_SET);
        write(fd, buf, block_size);
        cerr<<(char*)buf<<endl;
        free(buf);
        close(fdf);
        update_sblock();
    }
    catch(const char* err){cerr<<err<<endl;}
}

int Storage::take_free_inode(void)
{
    for (int i=0; i<i_arr_size-1; i++)
    {
        if (i_arr[i]!=0)
        {
            int n=i_arr[i];
            i_arr[i]=0;
            i_free--;
            update_sblock();
            return n;
        }
    }
    throw("Problem with free inodes");
}

void Storage::add_free_inode(int n)
{
    for (int i=0; i<i_arr_size-1; i++)
    {
        if (i_arr[i]==0)
        {
            i_arr[i]=n;
            i_free++;
            update_sblock();
            return;
        }
    }
    throw("Problem with free inodes");
}

Storage::Storage(const char* path):block_size(4096),file_size(10000),i_arr_size(256)
{
    try
    {
        fd=open("./info.log",O_RDONLY);
        if (fd==-1)
        {   
            fd=creat("./info.log",S_IRUSR);
            write(fd, path, sizeof(char)*strlen(path));
            s_path=(char*)malloc(sizeof(char)*(strlen(path)+1));
            strcpy(s_path,path);
            //cerr<<s_path<<endl;
            close(fd);
            fd=creat(s_path,S_IRUSR|S_IWUSR);
            if (fd==-1) throw("Storage Creation Error!");
            fd=open(s_path, O_RDWR);
            if (fd==-1) throw("Storage Opening Error!");
            void* buf=calloc(1, block_size);
            for (int i=0; i<file_size; i++) write(fd, buf, block_size);
            free(buf);
            i_free=i_arr_size-1;
            for (int i=0; i<i_arr_size-1; i++) i_arr[i]=i+1;
            //update_sblock();
            i_blocks=sizeof(inode)*i_arr_size/block_size+1;
            int b=file_size-1-i_blocks;
            for (int i=0; i<b; i++) add_free_block(file_size-1-i);
            for (int i=0; i<12; i++) inode[i]=0;
            inode[0]=take_free_block();
            //cerr<<"каталог блок: "<<inode[0]<<endl;
            lseek(fd, block_size, SEEK_SET);
            write(fd, inode, sizeof(inode));
            update_sblock();
        }
        else
        {
            s_path=(char*)malloc(sizeof(char));
            int i=1;
            while (read(fd, &s_path[i-1], sizeof(char)))
            {
                i++;
                s_path=(char*)realloc(s_path, i*sizeof(char));
            }       
            s_path[i]='\0';  
            close(fd);
            fd=open(s_path, O_RDWR);
            if (fd==-1) throw("Storage Opening Error!");
            read_sblock();
        }
    }
    catch(const char* err){cerr<<err<<endl;}
}

Storage::~Storage()
{
    update_sblock();
    //free(i_arr);
    //free(b_arr);
    free(s_path);
    close(fd);
}

void Storage::add_free_block(int n)
{
    for (int i=0; i<99; i++)
    {
        if (b_arr[i]==0)
        {
            b_arr[i]=n;
            b_free++;
            update_sblock();
            return;
        }
    }
    //if (b_arr[99]==0)
    {
        lseek(fd, n*block_size, SEEK_SET);
        write(fd, b_arr, sizeof(b_arr));
        for (int i=0; i<99; i++) b_arr[i]=0;
        b_arr[99]=n;
        b_free++;
        update_sblock();
        return;
    }
}

int Storage::take_free_block(void)
{
    for (int i=0; i<99; i++)
    {
        if (b_arr[i]!=0)
        {
            int n=b_arr[i];
            b_arr[i]=0;
            b_free--;
            update_sblock();
            return n;
        }
    }
    if (b_arr[99]!=0)
    {
        int n=b_arr[99];
        lseek(fd, n*block_size, SEEK_SET);
        read(fd, b_arr, sizeof(b_arr));
        b_free--;
        update_sblock();
        return n;
    }
    else throw("Problem with free blocks");
}

void Storage::read_sblock(void)
{
    read(fd, &block_size, sizeof(int));
    read(fd, &file_size, sizeof(int));
    read(fd, &i_arr_size, sizeof(int));
    read(fd, &i_free, sizeof(int));
    read(fd, &b_free, sizeof(int));
    read(fd, i_arr, sizeof(i_arr));
    read(fd, b_arr, sizeof(b_arr));
}
void Storage::update_sblock(void)
{
    lseek(fd, 0, SEEK_SET);
    write(fd, &block_size, sizeof(int));
    write(fd, &file_size, sizeof(int));
    write(fd, &i_arr_size, sizeof(int));
    write(fd, &i_free, sizeof(int));
    write(fd, &b_free, sizeof(int));
    write(fd, i_arr, sizeof(i_arr));
    write(fd, b_arr, sizeof(b_arr));
}
int main()
{
    Storage s;
    s.AddFile("./readme.txt","readme.txt");
    s.LoadFile("readme.txt","./2.txt");
    //s.ClearStorage();
    //s.AddFile("./readme.txt","readme.txt");
}
