#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <vector>

class USB {
public:
    char *label;
    char *UID;
    char *log;
    char *pin;
    USB(){
        label = new char[20];
        UID = new char[20];
        log = new char[50];
        pin = new char[50];
    }
    USB (const USB &a){
        label = new char[20];
        UID = new char[20];
        log = new char[50];
        pin = new char[50];
        strcpy(label,a.label);
        strcpy(UID,a.UID);
        strcpy(log,a.log);
        strcpy(pin,a.pin);
    }
    ~USB(){
        delete []label;
        delete []log;
        delete []UID;
        delete []pin;
    }
};

struct info_struct {
    int key;
    int ls,fd;
    int x;
    fd_set readfds;
};

class ACTION{
    static int N_GETUNLIM;
    int number;
public:
    int fd;
    char cmd[1024];
    int pos;
    USB token;
    ACTION (int i = 0, int j = 0) {fd = number = i; pos = 0;*cmd=0;}
    char* getlineunlim();
    int count_USB();//количество флешек вставленно
    void takeusbinf();//получаем информацию о флешке
    void do_command(struct info_struct *b);//выбирает кнопку из client1
    void do_encode(struct info_struct *b);//кнопка code - client1
    void do_decode(struct info_struct *b);//кнопка read - client2
    void do_delete(struct info_struct *b);//кнопка delete - client1
    void do_alert();//останов , но не факт что будет
    void makemasterkey();//создает мастер ключ
    char * shifrovat(char *adr);//шифрует диск
    void in_storage(char*str);//вставляем шимфр текст в виртуальную память
    void del_disk(char *adr);//удаляет все файлы по адресу, чтоб на компе хранился только зашифрованный текст
    char* from_storage(char *adr);//берет из виртуалки конкретный файл
    char* rasshifrovat(char *adr);//расшифровывает файл(что выдает пока непонятно)
    void makefile(char*str);//создает файл с содержанием стр
    void delete_storage();//удаляет виртуальную память
    void open_text(char*str);//открывает во втором клиенте результирующий файл, возможно создает файл, клиент его открывает выводит, а потом удаляет
};


int ACTION::N_GETUNLIM = 5;

std::vector <char*> data_usb(2),check(2);
char* ACTION::getlineunlim()                                 //функция считывает строку до \n
{
    char *p;
    int i = N_GETUNLIM, k = 0;
    if (!(p = (char*) malloc(i * sizeof(char)))) goto fail;
    fgets(&p[k], N_GETUNLIM, stdin);
    k = strlen(p);
    while (p[k-1]!='\n')
    {
        if ((k+N_GETUNLIM)>i)
        {
            i *= 2;
            if (!(p = (char*) realloc(p, i * sizeof(char)))) goto fail;
        }
        fgets(&p[k], N_GETUNLIM, stdin);
        k = strlen(p);
    }
    return p;
    fail:
        p = NULL;
        return p;
}

int ACTION::count_USB(){                                     //считает количество воткнутых флешек, ее же надо вставить в демон для проверки
    int pid1,pid2, fd1[2],fd2[2],numb = 0;
    pipe(fd1);
    pid1 = fork();
    if(!pid1){
        pipe(fd2);
        pid2 = fork();
        if(!pid2){
            dup2(fd2[1],1);
            close(fd1[1]);
            close(fd1[0]);
            close(fd2[0]);
            close(fd2[1]);
            execlp("blkid","blkid","-t","TYPE=vfat",NULL);
            return -1;
        }
        else {
        dup2(fd1[1],1);
        dup2(fd2[0],0);
        close(fd1[1]);
        close(fd1[0]);
        close(fd2[0]);
        close(fd2[1]);
        execlp("wc","wc","-l" ,NULL);
        return -1;
        }
    }
    else {
        wait(NULL);
        dup2(fd1[0],0);
        close(fd1[0]);
        close(fd1[1]);
        char * p;
        p = getlineunlim();
        int i,k;
        k = strlen(p);
        numb = 0;
        for (i = 0; i < k-1; ++i) numb = numb*10+p[i]- '0';
    }
    return numb;
}
void ACTION::takeusbinf(){                 //узнает информацию о воткнутой флешке, записывает ее в вектор data: 0)LABEL 1)UUID
    int pid,fd[2];
    pipe(fd);
    pid = fork();
    if(!pid){
         dup2(fd[1],1);
         close(fd[0]);
         close(fd[1]);
         execlp("blkid","blkid","-t" ,"TYPE=vfat",NULL);
    }
    else {
         dup2(fd[0],0);
         close(fd[0]);
         close(fd[1]);
         char * p,*q;
         int z,t,k,i = 0;
         p = getlineunlim();
         k = strlen(p);
         while(p[i++]!=' ');
         for (z = 0; z < 2; ++z){
            while(p[i++]!='"');
            t = i;
            q = new char[k];
            while(p[i]!='"'){
                q[i-t] = p[i];
                i++;
            }
            q[i-t] = '\0';
            i++;
            if (z == 0) strcpy(token.label,q);
            else strcpy(token.UID,q);
         }
         delete []q;
    }
}

