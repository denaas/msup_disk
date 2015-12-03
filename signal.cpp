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

using namespace std;
#define N_GETUNLIM 5
vector <char*> data_usb(2),check(2);
char *getlineunlim()                                 //функция считывает строку до \n
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

int count_USB(){                                     //считает количество воткнутых флешек, ее же надо вставить в демон для проверки
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


void takeusbinf(vector<char*> &data){                 //узнает информацию о воткнутой флешке, записывает ее в вектор data: 0)LABEL 1)UUID
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
            data[z] = q;
         }
    }
}

void repeat_function(int s)
{
    int flag = 0;

    flag = count_USB();
    if (flag == 1) {
        takeusbinf(check);
        //здесь нужно будет считывать данные стокена и с ними сравнивать сравнение ниже

        //if (!strcmp(data_usb[0],check[0]) && !strcmp(data_usb[1],check[1]));

    }
    else {
    }
    alarm(5);
    signal(SIGALRM, repeat_function);
}

int main()
{
    signal(SIGALRM, repeat_function);
    alarm(5);
    while (1)
    {
    }
    return 0;
}
