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

struct info_struct {
	int key;
    int ls,fd;
	int x;
	fd_set readfds;	
};

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

void do_delete(struct info_struct *b)
{
    int i = 0;
    char * str = new char[100];
    char * log = new char[100];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, log+i, 1) == 0) printf("read error\n");
    }
    while(log[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<log<<std::endl;
    std::cout<<pin<<std::endl;
     //код для работы функции здесь, если ошибка возвращать не окей
    strcpy(str,"Okey\0");
    write(b->fd,str,strlen(str)+1);
}

void do_encode(struct info_struct *b)
{
    int i = 0;
    char * str = new char[100];
    char * log = new char[100];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, log+i, 1) == 0) printf("read error\n");
    }
    while(log[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<log<<std::endl;
    std::cout<<pin<<std::endl;
     //код для работы функции здесь, если ошибка возвращать не окей
    strcpy(str,"Okey\0");
    write(b->fd,str,strlen(str)+1);
}

void do_decode(struct info_struct *b)
{
    int i = 0;
    char * str = new char[100];
    char * log = new char[100];
    char * pin = new char[100];
    do {
    if (read(b ->fd, str+i, 1) == 0) printf("read error\n");
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, log+i, 1) == 0) printf("read error\n");
    }
    while(log[i++] != '\0');
    i = 0;
    do {
    if (read(b ->fd, pin+i, 1) == 0) printf("read error\n");
    }
    while(pin[i++] != '\0');
    std::cout<<str<<std::endl;
    std::cout<<log<<std::endl;
    std::cout<<pin<<std::endl;
     //код для работы функции здесь, если ошибка возвращать не окей
    strcpy(str,"Okey\0");
    write(b->fd,str,strlen(str)+1);
}

void do_alert()
{
	const char buf1[] = "USB was removed!\n>";
	printf(buf1);
}


void do_command(struct info_struct *b, char * cmd)
{
	if(!strcmp(cmd,"\0")) {
		return;
	}
    if(!strcmp(cmd,"delete")) {
        do_delete(b);
		return;
	}
	if(!strcmp(cmd,"encode")) {
        do_encode(b);
		return;
	}
	if(!strcmp(cmd,"alert")) {
		do_alert();
		return;
	}
	if(!strcmp(cmd,"decode")) {
        do_decode(b);
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
    char  *str = new char[10];
    socklen_t alen;
	int max1 = 1;
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
        do_command(&all_info,str);
        if (read(fd, str, 1) == 0) close(fd);

	}
	return 0;
}
