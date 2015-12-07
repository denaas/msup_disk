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
            this->keydecoder();
            char *res=NULL;
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

void ACTION::do_encode(struct info_struct *b)           //кодируем диск
{
    char * str = new char[100];
    char * pin = new char[100];
	
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
            this->keydecoder();             //расшифровываем ключ
            char* res = this->shifrovat(str);           //шифруем
            this->in_storage(res);          //запихиваем в виртуальную память
            this->del_disk(str);            //удаляем с реального диска файлы
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

void ACTION::do_decode(struct info_struct *b)       //открытие зашифрованного файла
{
    char * str = new char[100];
    char * pin = new char[100];

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
			this->keydecoder();
            res = this->from_storage(str); //внутри функции должен быть адрес конкретного файла
            res = this->rasshifrovat(res);
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
}

int ACTION::ReadFromSocket(char *str)
{
	int rr;
	int pos = 0;
	do {
		rr = read(fd, str+pos, sizeof(str) - pos - 1);
		if (rr == -1){
			error_detected("read");
			return 1;
		}
		if (rr == 0){
			error_detected("lost connection");
			return 2;
		}
		pos+=rr;
		str[pos]= '\0';
	} while (!::is_n(str));
	for(int i = 0; str[i]; i++)
		if (str[i] == '\r'){
			str[i] = '\n';
			str[i+1] = '\0';
		}
	printf("%s",str);
	return 0; //no imput error handling, assume it's correct;
}

void ACTION::do_alert()
{
	const char buf1[] = "USB was removed!\n>";
	printf(buf1);
}

void ACTION::do_key(struct info_struct *b){         //создаем мастер-ключ
    std::cout<<"tut"<<std::endl;
    char str[128];
    char pin[128];
	
	write_client(fd,"Enter str:\n");
    ReadFromSocket(str);                        //считываем адрес
	write_client(fd,"Enter pin:\n");
    ReadFromSocket(pin);                        //считываем пин

    std::cout<<str<<std::endl;
    std::cout<<pin<<std::endl;
    int flag = 0;
    flag = this->count_USB();                   //проверяем количество вставленных флешек
    if (flag == 1) {
         global.takeusbinf_g();                 //задаем параметры флешки с которыми потом будет все работать
         std::cout<<global.flash.label<<' '<<global.flash.UID<<std::endl;
         global.makemasterkey(pin);             //функция создания
		 write_client(fd,"Okey\n");
    }
    else{
		write_client(fd,"Mistake\n");
    }
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
	int pos = c.pos;

	rr = read(c.fd, c.cmd+pos, sizeof(c.cmd) - pos - 1);
	if (rr == -1)
		error_detected("read");
	if (rr == 0)
		return 0;
	pos+=rr;
	c.pos = pos;
	c.cmd[pos] = '\0';
	for(int i = 0; c.cmd[i]; i++)
		if (c.cmd[i] == '\r'){
			c.cmd[i] = '\n';
			c.cmd[i+1] = '\0';
		}
	return rr;
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
