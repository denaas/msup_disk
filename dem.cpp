#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <netinet/in.h> // For OS X and Linux. Doesn't need for Free BSD
#include "header.h"
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include "types.hpp"

GLOBAL global;
char * command_line = new char[20];

void itoa(int n, char s[])
{
    sprintf(s,"%d",n);
}

bool is_n(char *s)
{
    for (int i = 0; s[i]; i++)
        if (s[i] == '\n')
            return true;
    return false;
}

void add_str(char to[], const char *str)
{
    int i = 0, j = 0;

    while(to[i])
        i++;
    while(str[j])
        to[i++] = str[j++];
    to[++i] = '\0';
}

void print_info(int total, int x)
{
    printf("Total connections:     %3d",total);
    printf("Value of the variable: %3d", x);
}

void error_detected(const char * s)
{
    perror(s);
    exit(1);
}

char *create_string(char *result_string, char *str1, char *str2, char *str3)

{
    strcpy(result_string, "/media/");
    strcat(result_string, str1);
    strcat(result_string, "/");
    strcat(result_string, str2);

    strcat(result_string, "/");
    strcat(result_string, str3);

    return result_string;
}

int count_words(char *s)
{
    int i,out=1,nw=0;

    for (i = 0; s[i]; i++) {
        if (s[i] == ' ')
            out = 1;
        else if (out) {
            out = 0;
            nw++;
        }
    }
    return nw;
}

int word_length(char *s)
{
    int i;

    for (i = 0; s[i]!= ' ' && s[i] != '\n' && s[i] != '\r'; i++)
    {}
    return i;
}

void GLOBAL::makemasterkey(char*pas,char *adr)//global.label,global.UID
{
    make_token_file(pas,flash.UID,adr);
}


char* ACTION::shifrovat(char *str,char *adr) //задается адрес для шифрования
{
    encrypt(token.pin,token.UID,str,adr);
    return NULL;
}
void ACTION::in_storage(char *str){}//вставляем шимфр текст в виртуальную память
char* ACTION::from_storage(char *adr){ return NULL;}//берет из виртуалки конкретный файл
char* ACTION::rasshifrovat(char *str,char *adr) //расшифровывает файл(что выдает пока непонятно)
{
    decrypt(token.pin,token.UID,str,adr);
    return NULL;
}
void ACTION::makefile(char*str){}//создает файл с содержанием стр
void ACTION::delete_storage(){}//удаляет виртуальную память
void ACTION::open_text(char*str){} //открывает во втором клиенте результирующий файл, возможно создает файл, клиент его открывает выводит, а потом удаляет

void ACTION::del_disk(const char *adr) //удаление файлов из диска
{
    struct stat s;
    lstat(adr, &s);
    if (!S_ISDIR(s.st_mode))
    {
        if (unlink(adr))
        {
            std::cout << "No such file or directory" << std::endl;
        }
        return;
    }
    DIR *d;
    struct dirent *dd;
    char name[PATH_MAX];
    if (!(d = opendir(adr)))
    {
        std::cout << "No such file or directory" << std::endl;
        return;
    }
    while((dd = readdir(d)))
    {
        if (!strcmp (dd->d_name,".") || !strcmp(dd->d_name,".."))
            continue;
        snprintf(name, PATH_MAX, "%s%s%s", adr, "/", dd->d_name);
        lstat(name, &s);
        if (S_ISDIR(s.st_mode))
        {
            del_disk(name);
            rmdir(name);
        }
        else
        {
            unlink(name);
        }
    }
    closedir(d);
    rmdir(adr);
}

void repeat_function(int s)         //функция проверки, что флешка внутри
{
    int flag = 0;
    ACTION help;
    flag = help.count_USB();
    if (flag == 1) {
        help.takeusbinf();
        if (!strcmp(help.token.label,global.flash.label) && !strcmp(help.token.UID,global.flash.UID))  {
            alarm(5);
            signal(SIGALRM, repeat_function);
        }
        else help.do_alert();
    }
    else {
        help.do_alert();
    }
}

void write_client(int fd,const char *str)
{
    int wc;

    wc = write(fd, str, strlen(str) + 1);
    if (wc == -1)
        error_detected("write");
}



void ACTION::do_delete(struct info_struct *b)
{
    char * str = new char[100];
    char * pin = new char[100];
    char * adr = new char[50];
    ReadFromSocket(str);                            //считываем адрес
    ReadFromSocket(pin);                            //считываем пин

    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    this->token.pin = pin;
    int flag = 0;
    flag = count_USB();                  //проверяем количество вставленных флешек
    if (flag == 1) {
        this->takeusbinf();                    //берем параметры вставленной флешки и потом сравниваем их с глобальными(первоначальными)
        if (!strcmp(this->token.label,global.flash.label) && !strcmp(this->token.UID,global.flash.UID)) {
            char *res=NULL;
            create_string(adr,command_line,global.flash.label,"token.txt");

            res = this->from_storage(res); //внутри функции должен быть адрес конкретного файла
            res = this->rasshifrovat(str,adr);
            makefile(res);           //создает файл с содержанием res  в папке str
            delete_storage();       //удаляет виртуальную память
            write_client(fd,"Okey\n");

        }
        else{
            write_client(fd,"Mistake\n");
        }
    }
    else{
       write_client(fd,"Mistake\n");
    }
        delete []adr;
}

void ACTION::do_encode(struct info_struct *b)           //кодируем диск
{
    char * str = new char[100];
    char * pin = new char[100];
    char * adr = new char[50];
    ReadFromSocket(str);                            //считываем адрес
    ReadFromSocket(pin);                            //считываем пин

    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    this->token.pin = pin;
    int flag = 0;
    flag = this->count_USB();                  //проверяем количество вставленных флешек
    if (flag == 1) {
        this->takeusbinf();                    //берем параметры вставленной флешки и потом сравниваем их с глобальными(первоначальными)
        std::cout<<this->token.label<<' '<<global.flash.label<<' '<<this->token.UID<<' '<<global.flash.UID<<std::endl;
        if (!strcmp(this->token.label,global.flash.label) && !strcmp(this->token.UID,global.flash.UID)) {
            create_string(adr,command_line,global.flash.label,"token.txt");

            char* res = this->shifrovat(str,adr);           //шифруем
            this->in_storage(res);          //запихиваем в виртуальную память
            //this->del_disk(str);            //удаляем с реального диска файлы
            write_client(fd,"Okey\n");
        }
        else{
            write_client(fd,"Mistake\n");
        }
    }
    else{
        write_client(fd,"Mistake\n");
    }
        delete []adr;
}

void ACTION::do_decode(struct info_struct *b)       //открытие зашифрованного файла
{
    char * str = new char[100];
    char * pin = new char[100];
    char * adr = new char[50];
    ReadFromSocket(str);                            //считываем адрес
    ReadFromSocket(pin);                            //считываем пин

    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    this->token.pin = pin;
    int flag = 0;
    flag = this->count_USB();                   //проверяем количество вставленных флешек
    if (flag == 1) {
        this->takeusbinf();                     //берем параметры вставленной флешки и потом сравниваем их с глобальными(первоначальными)
        if (!strcmp(this->token.label,global.flash.label) && !strcmp(this->token.UID,global.flash.UID)) {
            char *res;
            res = this->from_storage(str); //внутри функции должен быть адрес конкретного файла
            create_string(adr,command_line,global.flash.label,"token.txt");
            res = this->rasshifrovat(str,adr);
            this->open_text(res); //открывает во втором клиенте результирующий файл, возможно создает файл, клиент его открывает выводит, а потом удаляет
            write_client(fd,"Okey\n");
        }
        else{
            write_client(fd,"Mistake\n");
        }
    }
    else{
        write_client(fd,"Mistake\n");
    }
    delete []adr;
}

int ACTION::ReadFromSocket(char *str)
{
    int rr;

    int pos = 0;
    do {
        rr = read(fd, str+pos, 1);
        if (rr == -1){
            error_detected("read");
            return -1;
        }
        if (rr == 0){
            error_detected("lost connection");
            return 0;
        }
    } while (str[pos++] !='\0');
    return 1; //no imput error handling, assume it's correct;
}

void ACTION::do_alert()
{
    const char buf1[] = "USB was removed!\n>";
    printf(buf1);
}

void ACTION::do_key(struct info_struct *b){         //создаем мастер-ключ
    char str[128];
    char pin[128];
    char * adr = new char[50];
    ReadFromSocket(str);                        //считываем адрес
    ReadFromSocket(pin);                        //считываем пин

    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    int flag = 0;
    flag = this->count_USB();                   //проверяем количество вставленных флешек
    if (flag == 1) {
         global.takeusbinf_g();                 //задаем параметры флешки с которыми потом будет все работать
         std::cout<<global.flash.label<<' '<<global.flash.UID<<std::endl;

         create_string(adr,command_line,global.flash.label,"token.txt");

         global.makemasterkey(pin,adr);             //функция создания
         write_client(fd,"Okey\n");
    }
    else{
        write_client(fd,"Mistake\n");
    }
    delete []adr;
}

void ACTION::do_command(struct info_struct *b)  //выбор выполняемой команды
{

    if(!*cmd) {
        return;
    }

    if(!strcmp(cmd,"delete\n")) {
        this-> do_delete(b);
        return;
    }
    if(!strcmp(cmd,"decode\n")) {
        this-> do_decode(b);
        return;
    }
    if(!strcmp(cmd,"encode\n")) {
        this-> do_encode(b);;
        return;
    }
    if(!strcmp(cmd,"key\n")) {
        this->do_key(b);
        return;
    }
}

void print_new(int n)
{
    printf("New client with fd = %d has been connected!\n",n);
}

void print_old(int old, int n)
{
    printf("A client with fd = %d has been disconnected! Number of connections: %d\n",old,n);
}

int start_listen(int port)
{
    struct sockaddr_in addr;
    int ls, opt = 1;
    ls = socket(AF_INET,SOCK_STREAM, 0);
    if (ls == -1)
        error_detected("ls");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    setsockopt(ls,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (0 != bind(ls, (struct sockaddr *) &addr, sizeof(addr)))
        error_detected("bind");
    if (-1 == listen(ls,5))
        error_detected("listen");
    return ls;
}

int read_client(ACTION &c)
{
    int rr;
    int i = 0;
    do {
        rr =  read(c.fd,c.cmd+i,1);
        if (rr == -1)
            error_detected("read");
        if (rr == 0)
            return 0;
    }
    while(c.cmd[i++] != '\0');
        return rr;
}

int main(int argc,const char **argv)
{
    struct info_struct all_info;
    //int port = atoi(argv[1]);
    strcpy(command_line,argv[1]);
    int port = 1200;
    std::vector<ACTION> client;

    ACTION TrueCrypt;

    int ls = start_listen(port);
    all_info.ls = ls; //listening socket
    printf("Server is ready. Maximum number of sockets hasn't beed limited\n");
    for (;;) { 		//MAIN LOOP
        int max_d = ls;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(ls, &readfds);
        for (int i = 0; i < client.size(); i++){
            FD_SET(client[i].fd, &readfds);
            if (client[i].fd > max_d)
                max_d = client[i].fd;
        }
        int res = select(max_d + 1, &readfds, NULL, NULL, NULL);
        if (res < 1) {
            if (errno != EINTR)
                error_detected("select");
            else
                printf("It's just the signal, don't worry!");
            continue;
        }
        if (FD_ISSET(ls, &readfds)) {
            int sfd = accept(ls,0,0);
            client.insert(client.end(), ACTION(sfd));
            print_new(sfd);
        }
        for (int i = 0; i < client.size(); i++ ) {
            if (FD_ISSET(client[i].fd, &readfds)) {
                int rr = read_client(client[i]);
                if (rr == 0) {
                    shutdown(client[i].fd,2);
                    close(client[i].fd);
                    print_old(client[i].fd, client.size()-1);
                    client.erase(client.begin() + i);
                } else {
                        printf("Client%d (fd = %d) wrote: %s",
                                        i+1,client[i].fd,client[i].cmd);
                    if (is_n(client[i].cmd)){
                        client[i].do_command(&all_info);
                        client[i].cmd[0]='\0';
                        client[i].pos=0;
                    }
                }
            }
        }
    }
    return 0;
}
