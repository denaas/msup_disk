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

GLOBAL global;

void reverse(char s[])
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void itoa(int n, char s[])
{
	int i,sign;
	
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n %10 + '0';
	} while ((n /= 10)> 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void itoa2(int n, char s[])
{
	int i,sign;
	
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n %10 + '0';
	} while ((n /= 10)> 0);
	if (sign < 0)
		s[i++] = '-';
	while (i<11) {
		s[i++] = ' ';
	}
	s[i] = '\0';
	reverse(s);
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

int is_n(char *s)
{
	int i;
	for (i = 0; s[i]; i++)
		if (s[i] == '\n')
			return 1;
	return 0;
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
void GLOBAL::makemasterkey(char*pin){} //global.label,global.UID
char* ACTION::shifrovat(char *adr){}//задается адрес для шифрования
void ACTION::in_storage(char *str){}//вставляем шимфр текст в виртуальную память
void ACTION::del_disk(char *adr){} //удаление файлов из диска
char* ACTION::from_storage(char *adr){}//берет из виртуалки конкретный файл
char* ACTION::rasshifrovat(char *adr){}//расшифровывает файл(что выдает пока непонятно)
void ACTION::makefile(char*str){}//создает файл с содержанием стр
void ACTION::delete_storage(){}//удаляет виртуальную память
void ACTION::open_text(char*str){} //открывает во втором клиенте результирующий файл, возможно создает файл, клиент его открывает выводит, а потом удаляет
void ACTION::keydecoder(){}//расшифровывает мастерключ token.pin,token.label,token.UID

void repeat_function(int s)
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





void ACTION::do_delete(struct info_struct *b)
{
    int i = 0;
    char * str = new char[100];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    this->token.pin = pin;
    int flag = 0;
    flag = count_USB();
    if (flag == 1) {
        this->takeusbinf();
        if (!strcmp(this->token.label,global.flash.label) && !strcmp(this->token.UID,global.flash.UID)) {
            this->keydecoder();
            char *res;
            //for(по всем файла из папки - str)
            {
                res = this->from_storage(res); //внутри функции должен быть адрес конкретного файла
                res = this->rasshifrovat(res);
                makefile(res);           //создает файл с содержанием res  в папке str
            }
            delete_storage();       //удаляет виртуальную память
            strcpy(str,"Okey\0");
            write(b->fd,str,strlen(str)+1);
        }
        else{
            strcpy(str,"Mistake\0");
            write(b->fd,str,strlen(str)+1);
        }
    }
    else{
        strcpy(str,"Mistake\0");
        write(b->fd,str,strlen(str)+1);
    }
}

void ACTION::do_encode(struct info_struct *b)
{
    int i = 0;
    char * str = new char[100];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    this->token.pin = pin;
    int flag = 0;
    flag = this->count_USB();
    if (flag == 1) {
        this->takeusbinf();
        std::cout<<this->token.label<<' '<<global.flash.label<<' '<<this->token.UID<<' '<<global.flash.UID<<std::endl;
        if (!strcmp(this->token.label,global.flash.label) && !strcmp(this->token.UID,global.flash.UID)) {
            this->keydecoder();
            char* res = this->shifrovat(str);
            this->in_storage(res);
            this->del_disk(str);
            strcpy(str,"Okey\0");
            write(b->fd,str,strlen(str)+1);
        }
        else{
            strcpy(str,"Mistake\0");
            write(b->fd,str,strlen(str)+1);
        }
    }
    else{
        strcpy(str,"Mistake\0");
        write(b->fd,str,strlen(str)+1);
    }
}

void ACTION::do_decode(struct info_struct *b)
{
    int i = 0;
    char * str = new char[100];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    this->token.pin = pin;
    int flag = 0;
    flag = this->count_USB();
    if (flag == 1) {
        this->takeusbinf();
        if (!strcmp(this->token.label,global.flash.label) && !strcmp(this->token.UID,global.flash.UID)) {
            char *res;
            this->keydecoder();
            res = this->from_storage(str); //внутри функции должен быть адрес конкретного файла
            res = this->rasshifrovat(res);
            this->open_text(res); //открывает во втором клиенте результирующий файл, возможно создает файл, клиент его открывает выводит, а потом удаляет
            strcpy(str,"Okey\0");
            write(b->fd,str,strlen(str)+1);
        }
        else{
            strcpy(str,"Mistake\0");
            write(b->fd,str,strlen(str)+1);
        }
    }
    else{
        strcpy(str,"Mistake\0");
        write(b->fd,str,strlen(str)+1);
    }
}

void ACTION::do_alert()
{
	const char buf1[] = "USB was removed!\n>";
	printf(buf1);
}
void ACTION::do_key(struct info_struct *b){
    int i = 0;
    char * str = new char[5];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    int flag = 0;
    flag = this->count_USB();
    if (flag == 1) {
          global.takeusbinf_g();
          std::cout<<global.flash.label<<' '<<global.flash.UID<<std::endl;
          global.makemasterkey(pin);
          strcpy(str,"Okey\0");
          write(b->fd,str,strlen(str)+1);
    }
    else{
        strcpy(str,"Mistake\0");
        write(b->fd,str,strlen(str)+1);
    }
    delete []pin;
}

void ACTION::do_command(struct info_struct *b, char * cmd)
{
	if(!strcmp(cmd,"\0")) {
		return;
	}
    if(!strcmp(cmd,"delete\0")) {
        this-> do_delete(b);
		return;
	}
    if(!strcmp(cmd,"encode\0")) {
        this-> do_encode(b);
		return;
	}
    if(!strcmp(cmd,"key\0")) {
        this->do_key(b);
		return;
	}
}

void print_new(int n)
{
	printf("New client with fd = %d has been connected!\n",n);
}

void print_old(int old)
{
	printf("A client with fd = %d has been disconnected!\n",old);
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

void before_start(struct info_struct *b)
{
	char buf[1024]="\0";
	int wc;

	add_str(buf,"Daemon has been started!\n");
	add_str(buf,"Identify yourself\n>");
	wc = write(b->ls, buf, strlen(buf) + 1);
    if (wc == -1)
		error_detected("write");
}

int main(int argc,char **argv)
{
	struct info_struct all_info;
    struct sockaddr_in addr;
    int fd,i;
    ACTION TrueCrypt;
    char  *str = new char[10];
    socklen_t alen;
    int max1 = 2;
	printf("Server is ready. Maximum number of sockets is %d\n",max1);
    all_info.ls = start_listen(atoi(argv[2]));
    //before_start(&all_info);
	for (;;) {
        alen = sizeof(addr);
        if ((fd = accept(all_info.ls, (struct sockaddr*) &addr,&alen)) < 0){std::cout<<"tuagat"<<std::endl;
            error_detected("accept");}
        all_info.fd = fd;
        i = 0;
        do {
        if (read(fd, str+i, 1) == 0) printf("read error\n");
        }
        while(str[i++] != '\0');
        if(!strcmp(str,"client_1\0")) {
            int i = 0;
            do {
            if (read(fd, str+i, 1) == 0) printf("read error\n");
            }
            while(str[i++] != '\0');
            ACTION client(1);
            client.do_command(&all_info,str);
        }
        if(!strcmp(str,"client_2\0")) {
            ACTION client(2);
            client.do_decode(&all_info);
        }
        if (read(fd, str, 1) == 0) close(fd);

	}
	return 0;
}
