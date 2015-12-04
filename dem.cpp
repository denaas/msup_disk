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
	sprintf(s,"%d",n);	
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

char* ACTION::shifrovat(char *adr){ return NULL;}//задается адрес для шифрования
void ACTION::in_storage(char *str){}//вставляем шимфр текст в виртуальную память
void ACTION::del_disk(char *adr){} //удаление файлов из диска
char* ACTION::from_storage(char *adr){ return NULL;}//берет из виртуалки конкретный файл
char* ACTION::rasshifrovat(char *adr){ return NULL;}//расшифровывает файл(что выдает пока непонятно)
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
    std::cout<<"tut"<<std::endl;
    int i = 0;
    char * str = new char[5];
    char * pin = new char[100];
    do {
    if (read(fd, str+i, 1) == -1) printf("read error\n");
       std::cout<<"tut"<<std::endl;
    }
    while(str[i++] != '\0');
    i = 0;
    do {
    if (read(fd, pin+i, 1) == 0) printf("read error\n");
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

void ACTION::do_command(struct info_struct *b)
{
    /*if(*cmd) {
		return;
    }*/
    if(!strcmp(cmd,"delete")) {
        this-> do_delete(b);
		return;
    }
    if(!strcmp(cmd,"decode")) {
        this-> do_decode(b);
        return;
    }
    if(!strcmp(cmd,"encode")) {
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

int read_client(ACTION &c)
{
		int rr;
		int pos = c.pos;

		rr = read(c.fd, c.cmd+pos, sizeof(c.cmd) - pos - 1);
		if (rr == -1)
			error_detected("read");
		if (rr == 0)
			return 0;
		pos+=rr;
		c.pos = pos;
        c.cmd[pos] = '\0';
		return rr;
}

bool is_n(char *s)
{
	for (int i = 0; s[i]; i++)
		if (s[i] == '\n')
			return true;
	return false;
}

int main(int argc,const char **argv)
{
	struct info_struct all_info;
	int port = atoi(argv[1]);
	std::vector<ACTION> client;

	ACTION TrueCrypt;
	
    int ls = start_listen(port);
    all_info.ls = ls; //listening socket
	printf("Server is ready. Maximum number of sockets hasn't beed limited\n");
    //before_start(&all_info);
	//all_info.fd = fd;
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
                        printf("Client%d (fd = %d) wrote: %s",i+1,client[i].fd,client[i].cmd);
					if (is_n(client[i].cmd)){
						client[i].do_command(&all_info);
						client[i].cmd[0]='\0';
						client[i].pos=0;
					}
				}
			}
		}
	}
/*   
	alen = sizeof(addr);
        if ((fd = accept(all_info.ls, (struct sockaddr*) &addr,&alen)) < 0){
			std::cout<<"tuagat"<<std::endl; // What is it? O_o
            error_detected("accept");
		}

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
	*/

	return 0;
}
