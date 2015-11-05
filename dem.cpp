#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

struct bankir {
	int start;
	int level;
	int month;
	int players;
	int ls;
	int x;
	int max;
	int raw_amount;
	int raw_price;
	int product_amount;
	int product_price;
	fd_set readfds;	
};

struct players {
	int number;
	int money;
	int raw;
	int product;
	int product_to_make;
	int factory;
	int buildings[6];
	int state;
	int fd;
	int turn;
	char buf[1024];
	int i;
	char name[128];
	char **command;
	int buy_price;
	int buy_amount;
	int sell_price;
	int sell_amount;
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

int read_player(struct players *player,int n)
{
	int rr;
	char *buf = player[n].buf;
	int pos = player[n].i;

	rr = read(player[n].fd, buf+pos, sizeof(player[n].buf) - pos - 1);
	if (rr == -1)
		error_detected("read");
	if (rr == 0)
		return 0;
	pos += rr;
	player[n].i = pos;
	buf[pos] = '\0';
	return rr;
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
char **command_make(char **old, char *s)
{
	int nw,i=0,j,a,wl;
	char **temp,*temp_word;

	nw = count_words(s);
	temp = malloc(sizeof(*temp) * (nw + 1));
	for (a=0; a<nw; a++) {
		while (s[i] == ' ')
			i++;
		wl = word_length(s+i);
		temp_word = malloc(sizeof(char) * wl + 1);
		for (j = 0;j<wl && (s[i]!=' ' || s[i]!='\n'); j++) {
			temp_word[j]=s[i];
			i++;
		}
		temp_word[j]= '\0';
		temp[a]= temp_word;
	}
	temp[nw] = NULL;	
	return temp;
}

char **command_delete(char **old)
{
	int i;

	for (i = 0; old[i] != NULL; i++)
		free(old[i]);
	free(old);
	return NULL;
}

void do_nothing(struct players *p,int i)
{
	const char* c=">";
	if (0 >= write(p[i].fd,c,strlen(c)+1))
		printf("write error to fd = %d",p[i].fd);

}

int active_players(struct bankir *b, struct players *p)
{
	int i,s=0;
	for (i = 0; i < b[0].players; i++)
		if (p[i].state > 0)
			s += 1;
	return s;
}

void do_market(struct bankir *b, struct players *p, int i)
{
	char num_str[10]="\0",buf[1024]="\0";

	add_str(buf,"Current month:    ");
	itoa(b[0].month,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Players still active:\n");
	add_str(buf,"%                     ");
	itoa(active_players(b,p),num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Bank sells: items    min.price\n");
	add_str(buf,"%           ");
	itoa(b[0].raw_amount,num_str);
	add_str(buf,num_str);
	add_str(buf,"         ");
	itoa(b[0].raw_price,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Bank buys:  items    max.price\n");
	add_str(buf,"%           ");
	itoa(b[0].product_amount,num_str);
	add_str(buf,num_str);
	add_str(buf,"         ");
	itoa(b[0].product_price,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n>");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}

void do_info(struct bankir *b, struct players *p,int i)
{
	char num_str[10]="\0",buf[1024]="\0";
	int n,j,no_buildings=1;

	if (p[i].command[1]!=NULL) {
		if ((n = atoi(p[i].command[1])) > b[0].players || n < 1) {
			add_str(buf,"Can't get info about ");
			add_str(buf,p[i].command[1]);
			add_str(buf," player.\n>");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
			return;
		} else {
			n -= 1;
		}
	} else {
		n = i;
	}
	add_str(buf,"****    Information about ");
	add_str(buf,p[n].name);
	add_str(buf,"    ****\n");
	add_str(buf,"Money:        ");
	itoa(p[n].money,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Raw units:    ");
	itoa(p[n].raw,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Products:     ");
	itoa(p[n].product,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Factories:    ");
	itoa(p[n].factory,num_str);
	add_str(buf,num_str);
	add_str(buf,"\n");
	add_str(buf,"Buildings:    ");
	for (j=5; j>=1; j--) {
		if (p[n].buildings[j]!= 0) {
			no_buildings = 0;
			itoa(p[n].buildings[j],num_str);
			add_str(buf,num_str);
			add_str(buf,"(months left: ");
			itoa(j,num_str);
			add_str(buf,num_str);
			add_str(buf,")  ");
		}
	}
	if (no_buildings)
		add_str(buf,"0");
	add_str(buf,"\n>");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}

void do_chat(struct bankir *b, struct players *p,int i)
{
	char buf[1024]="\0";
	int j;

	add_str(buf,p[i].name);
	add_str(buf," says: [ ");
	for (j=1; p[i].command[j]!= '\0'; j++){
		add_str(buf,p[i].command[j]);
		add_str(buf," ");
	}
	add_str(buf,"]\n>");
	for(j=0;j<b[0].players;j++){
		if (0 >= write(p[j].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[j].fd);
	}
}

void do_turn(struct players *p,int i)
{
	const char buf1[] = "End of your turn. Waiting for other players...\n>";
	const char buf2[] = "Sorry but you can't play anymore.\n>";
	if (p[i].state < 0){
		if (0 >= write(p[i].fd,buf2,strlen(buf2)+1))
			printf("write error to fd = %d",p[i].fd);
	} else {
		p[i].turn = 1;
		if (0 >= write(p[i].fd,buf1,strlen(buf1)+1))
			printf("write error to fd = %d",p[i].fd);
	}
}

void do_buy(struct bankir *b, struct players *p,int i)
{
	char num_str[10]="\0",buf[1024]="\0";
	int amount,price;

	if (p[i].state < 0){
		add_str(buf,"Sorry but you can't play anymore.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
			return;
	}
	if (p[i].command[1]==NULL) {
			add_str(buf,p[i].command[0]);
			add_str(buf,": wrong command syntax. Must be '");
			add_str(buf,"buy <amount> <price>'\n>");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
			return;
	} else if (p[i].command[2]==NULL) {
			add_str(buf,p[i].command[0]);
			add_str(buf,": wrong command syntax. Must be '");
			add_str(buf,"buy <amount> <price>'\n>");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
			return;
	}
	amount = atoi(p[i].command[1]);
	price = atoi(p[i].command[2]);
	if (amount > b[0].raw_amount || amount <= 0) {
		add_str(buf,"In this month you can buy from 1 to ");
		itoa(b[0].raw_amount,num_str);
		add_str(buf,num_str);
		add_str(buf," units. Try again.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	if (price < b[0].raw_price) {
		add_str(buf,"In this month you have to pay not less than ");
		itoa(b[0].raw_price,num_str);
		add_str(buf,num_str);
		add_str(buf,"$ for 1 unit. Try again.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	if (p[i].money - price * amount < 0) {
		add_str(buf,"Sorry but you don't have enough money to ");
		add_str(buf,"buy all this stuff. You have ");
		itoa(p[i].money,num_str);
		add_str(buf,num_str);
		add_str(buf,"$ but you need ");
		itoa(price*amount,num_str);
		add_str(buf,num_str);
		add_str(buf,"$ to buy everything you want.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	p[i].buy_amount = amount;
	p[i].buy_price = price;
	add_str(buf,"You query to buy ");
		itoa(amount,num_str);
		add_str(buf,num_str);
		add_str(buf," for ");
		itoa(price,num_str);
		add_str(buf,num_str);
		add_str(buf,"$ was sent to the bank.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
}

void do_sell(struct bankir *b, struct players *p,int i)
{
	char num_str[10]="\0",buf[1024]="\0";
	int amount,price;

	if (p[i].state < 0){
		add_str(buf,"Sorry but you can't play anymore.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
			return;
	}
	if (p[i].command[1]==NULL) {
			add_str(buf,p[i].command[0]);
			add_str(buf,": wrong command syntax. Must be '");
			add_str(buf,"sell <amount> <price>'\n>");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
			return;
	} else if (p[i].command[2]==NULL) {
			add_str(buf,p[i].command[0]);
			add_str(buf,": wrong command syntax. Must be '");
			add_str(buf,"sell <amount> <price>'\n>");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
			return;
	}
	amount = atoi(p[i].command[1]);
	price = atoi(p[i].command[2]);
	if (amount > b[0].product_amount || amount <= 0) {
		add_str(buf,"In this month you can sell from 1 to ");
		itoa(b[0].product_amount,num_str);
		add_str(buf,num_str);
		add_str(buf," products. Try again.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	if (price > b[0].product_price) {
		add_str(buf,"In this month you can't ask for more than ");
		itoa(b[0].product_price,num_str);
		add_str(buf,num_str);
		add_str(buf,"$ for 1 product. Try again.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	if (p[i].product - amount < 0) {
		add_str(buf,"Sorry but you don't have enough product to ");
		add_str(buf,"sell. You have ");
		itoa(p[i].product,num_str);
		add_str(buf,num_str);
		add_str(buf," product(s) but\nyou wanted to sell ");
		itoa(amount,num_str);
		add_str(buf,num_str);
		add_str(buf," product(s).\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	p[i].sell_amount = amount;
	p[i].sell_price = price;
	add_str(buf,"You query to sell ");
		itoa(amount,num_str);
		add_str(buf,num_str);
		add_str(buf," for ");
		itoa(price,num_str);
		add_str(buf,num_str);
		add_str(buf,"$ was sent to the bank.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
}

void do_prod(struct players *p,int i)
{
	int req;
	char buf[1024]="\0";

	if (p[i].state < 0){
		add_str(buf,"Sorry but you can't play anymore.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
			return;
	}
	req = p[i].product_to_make + 1;
	if (req > p[i].factory) {
		add_str(buf,"You don't have enough factories to produce.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	if (req > p[i].raw) {
		add_str(buf,"You don't have enough raw units to produce.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	if (p[i].money - 2000 < 0){
		add_str(buf,"You don't have enough money to produce.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	p[i].money -= 2000;
	p[i].product_to_make = req;
	add_str(buf,"You've been charged 2000$ for production.\n");
	add_str(buf,"Product will be done at the end of the month.\n>");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}

void do_build(struct players *p,int i)
{
	char buf[1024]="\0";

	if (p[i].state < 0){
		add_str(buf,"Sorry but you can't play anymore.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
			return;
	}
	if (p[i].money - 2500 < 0){
		add_str(buf,"You don't have enough money to build.\n>");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		return;
	}
	p[i].money -= 2500;
	p[i].buildings[5] += 1;
	add_str(buf,"You've been charged 2500$ for the building.\n");
	add_str(buf,"The building will reform to a factory on 5th month.\n>");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}

//help info for daemon
void do_help(struct players *p, int i)
{
	char buf[1024]="\0";

	add_str(buf,"******************************************************\n");
	add_str(buf,"COMMAND                     DESCRIPTION               \n");
	add_str(buf," info [number]         get partners' information      \n");
	add_str(buf," help                  get help                       \n");
	add_str(buf,"             RULES:                                   \n");
	add_str(buf,"******************************************************\n");
	add_str(buf,"\n>");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}

void do_who(struct bankir *b, struct players *p,int i)
{
	char buf[1024]="\0";
	int j;

	add_str(buf,"Waiting for:  \n");
	for (j=0; j<b[0].players; j++) {
		if (p[j].turn != 1) {
			add_str(buf,"            ");
			add_str(buf,p[j].name);
			add_str(buf,"\n");
		}
	}
	add_str(buf,"\n>");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}


void do_command(struct bankir *b, struct players *p,int i)
{
	if(!strcmp(p[i].command[0],"\0")) {
		do_nothing(p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"market")) {
		do_market(b,p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"info")) {
		do_info(b,p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"buy")) {
		do_buy(b,p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"sell")) {
		do_sell(b,p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"prod")) {
		do_prod(p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"turn")) {
		do_turn(p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"chat")) {
		do_chat(b,p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"build")) {
		do_build(p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"help")) {
		do_help(p,i);
		return;
	}
	if(!strcmp(p[i].command[0],"who")) {
		do_who(b,p,i);
		return;
	}
	do_nothing(p,i);
}

void new_month_notification(struct bankir *b, struct players *p)
{
	char buf[1024]="\0", num[10]="\0";
	int j;
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	add_str(buf,"|               New month: ");
	itoa2(b[0].month,num);
	add_str(buf,num);
	add_str(buf,"             |\n");
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n>");
	for(j=0;j<b[0].players;j++){
		if (0 >= write(p[j].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[j].fd);
	}
}

void command(struct bankir *bank,struct players *p, int i)
{
	char buf[1024]="\0";
	
	if (!bank[0].start){
		add_str(buf,"The game hasn't been started!\n");
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
		p[i].i = 0;
		return;	
	}
	if (is_n(p[i].buf)){	
		p[i].command = command_make(p[i].command,p[i].buf);
		if(p[i].state == 1){
			strcpy(p[i].name,p[i].command[0]);
			p[i].state = 2;
			add_str(buf,"Thank you ");
			add_str(buf,p[i].name);
			add_str(buf,". Good luck!\n>");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
		} else {
			do_command(bank,p,i);	
		}
		p[i].i = 0;
		p[i].command = command_delete(p[i].command);
	}
}

struct players *
add_player(struct players *old, int *total, int new_sfd)
{

	struct players *temp;
	int i,j;
	temp = malloc(sizeof(*temp) * (*total + 1));
	for (i = 0; i < *total; i++)
		temp[i]  = old[i];
	temp[i].number = i+1;
	temp[i].money = 10000;
	temp[i].raw = 4;
	temp[i].product = 2;
	temp[i].product_to_make = 0;
	temp[i].factory = 2;
	for (j = 0; j <= 5;j++)
		temp[i].buildings[j] = 0;
	temp[i].state = 1;
	temp[i].fd = new_sfd;
	temp[i].i = 0;
	temp[i].command = NULL;
	temp[i].turn = 0;
	temp[i].buy_price=0;
	temp[i].buy_amount=0;
	temp[i].sell_price=0;
	temp[i].sell_amount=0;
	strcpy(temp[i].name,"Anonimus");
	if (*total > 0)
		free(old);
	(*total) += 1;
	return temp;
}

struct players *
del_player(struct players *old, int *total, int num_to_del)
{
	struct players *temp;
	int i,j=0;
	
	temp = malloc(sizeof(*temp) * (*total - num_to_del));
	for (i = 0; i < *total; i++)
		if (old[i].state)
			temp[j++] = old[i];
	free(old);
	*total -= num_to_del;
	return temp;
}

void print_digit_array(int *array, int size)
{
	int i;

	printf("The array[%d] contains following elements:\n",size);
	for (i = 0; i < size; i++)
		printf("%d%s",array[i], (i+1 != size) ? " ": "\n");
}

void pre_del(struct players *arr, int size, int num)
{
	int i;

	for (i = 0; i < size; i++)
		if (arr[i].fd == num)
			arr[i].state = 0;
}

void welcome(int fd, int new)
{
	char buf[1024]="\0";
	int wc;
	if (fd == new){
		add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
		add_str(buf,"|                Welcome!              |\n");
		add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	} else {
		add_str(buf,"New client has been connected.\n");
	}		
	wc = write(fd, buf, strlen(buf) + 1);
	if (wc == -1)
		error_detected("write");
}

void write_sorry(int fd)
{
	char buf[1024]="\0";
	int wc;

	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	add_str(buf,"|                      Sorry!                     |\n");
	add_str(buf,"| Maximum number of sockets have been connected!  |\n");
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	wc = write(fd, buf, strlen(buf) + 1);
	if (wc == -1)
		error_detected("write");
}

void print_new(int new, int total)
{
	printf("New client with fd = %d has been connected!\n",new);
	printf("Total number of clients is %d\n",total);
}

void print_old(int old, int total)
{
	printf("A client with fd = %d has been disconnected!\n",old);
	printf("Total number of clients is %d\n",total);
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

void my_select(struct bankir *bank, struct players *player)
{
	int i,max_d = bank[0].ls;
	FD_ZERO(&bank[0].readfds);
	FD_SET(bank[0].ls, &bank[0].readfds);
	for (i = 0; i < bank[0].players ; i++) {
		FD_SET(player[i].fd, &bank[0].readfds);
		if (player[i].fd > max_d)
			max_d = player[i].fd;
	}
	int res = select(max_d + 1, &bank[0].readfds, NULL, NULL, NULL);
	if (res < 1) {
		if ( errno != EINTR)
			error_detected("select");
		else
			printf("signal EINTR");
	}
}

int cmp_buy(struct players *p1, struct players *p2)
{
	return (*p2).buy_price - (*p1).buy_price;
}

int cmp_sell(struct players *p1, struct players *p2)
{
	return (*p1).sell_price - (*p2).sell_price;
}

int sellers(struct bankir *b, struct players *p)
{
	int i,temp=0;

	for (i = 0; i<b[0].players; i++)
		if (p[i].sell_amount>0)
			temp++;
	return temp;
}

int buyers(struct bankir *b, struct players *p)
{
	int i,temp=0;

	for (i = 0; i<b[0].players; i++)
		if (p[i].buy_amount>0)
			temp++;
	return temp;
}

void comply_sell(struct bankir *b, struct players *p,int fd)
{
	int i=0,ok=1,am,pr;
	char buf[1024] = "\0", num[10] = "\0";

	while (ok) {
		if (p[i].fd == fd){
			ok = 0;
			am=p[i].sell_amount;
			pr=p[i].sell_price;
			p[i].money += pr * am;
			p[i].product -= am;
		}
		i++;
	}
	printf(" %d products (%d$ each) bought from %s\n",am,pr,p[i-1].name);
	add_str(buf,"%                ");
	itoa2(am,num);
	add_str(buf,num);
	itoa2(pr,num);
	add_str(buf,num);
	add_str(buf,"     ");
	add_str(buf,p[i-1].name);
	add_str(buf,"\n");
	for(i=0;i<b[0].players;i++){
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
	}
}

void comply_sell_one(struct bankir *b, int *n, struct players *p,int fd)
{
	int r, i=0, ok=1;
	char buf[1024] = "\0", num[10] = "\0";

	while (ok) {
		if (p[i].fd == fd)
			ok = 0;
		i++;
	}
	i--;
	r = rand() % 2;
	if (r) {
		(*n)--;
		p[i].money += p[i].sell_price;
		p[i].product -= 1;
		printf(" 1 product (%d$) RANDOMLY",p[i].sell_price);
		printf(" bought from %s\n",p[i].name);
		add_str(buf,"%                ");
		itoa2(1,num);
		add_str(buf,num);
		itoa2(p[i].sell_price,num);
		add_str(buf,num);
		add_str(buf,"     ");
		add_str(buf,p[i].name);
		add_str(buf,"\n");
		for(i=0;i<b[0].players;i++){
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
		}
	} else {
		printf(" %s is unlucky today\n",p[i].name);
	}
}

void comply_buy(struct bankir *b, struct players *p,int fd)
{
	int i=0,ok=1,am,pr;
	char buf[1024] = "\0", num[10] = "\0";

	while (ok) {
		if (p[i].fd == fd){
			ok = 0;
			am=p[i].buy_amount;
			pr=p[i].buy_price;
			p[i].money -= pr * am;
			p[i].raw += am;
		}
		i++;
	}
	printf(" %d raw units (%d$ each) sold to %s\n",am,pr,p[i-1].name);
	add_str(buf,"%                ");
	itoa2(am,num);
	add_str(buf,num);
	itoa2(pr,num);
	add_str(buf,num);
	add_str(buf,"     ");
	add_str(buf,p[i-1].name);
	add_str(buf,"\n");
	for(i=0;i<b[0].players;i++){
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
	}
}

void comply_buy_one(struct bankir *b, int *n, struct players *p,int fd)
{
	int r, i=0, ok=1;
	char buf[1024] = "\0", num[10] = "\0";

	while (ok) {
		if (p[i].fd == fd)
			ok = 0;
		i++;
	}
	i--;
	r = rand() % 2;
	if (r) {
		(*n)--;
		p[i].money -= p[i].buy_price;
		p[i].raw += 1;
		printf(" Raw unit (%d$) RANDOMLY",p[i].buy_price);
		printf(" sold to %s\n",p[i].name);
		add_str(buf,"%                ");
		itoa2(1,num);
		add_str(buf,num);
		itoa2(p[i].buy_price,num);
		add_str(buf,num);
		add_str(buf,"     ");
		add_str(buf,p[i].name);
		add_str(buf,"\n");
		for(i=0;i<b[0].players;i++){
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
		}
	} else {
		printf(" %s is unlucky today\n",p[i].name);
	}
}

void auction_buy(struct bankir *b, struct players *p)
{
	int i,j=0,size,am,pr,max,sum;
	struct players *temp,*p_temp;
	char buf[1024]="\0",num[10]="\0";

	size = buyers(b,p);
	if (!size) {
		printf("Auction:  buy: no requests for buying raw units:\n");
		add_str(buf,"        BANK SOLD     amount      ");
		add_str(buf,"price     buyer\n");
		add_str(buf,"%                ");
		itoa2(0,num);
		add_str(buf,num);
		itoa2(0,num);
		add_str(buf,num);
		add_str(buf,"     ");
		add_str(buf,"nobody\n");
		for(i=0;i<b[0].players;i++){
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
		}
		return;
	}
	add_str(buf,"        BANK SOLD     amount      price     buyer\n");
	for(i=0;i<b[0].players;i++){
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
	}
	temp = malloc(sizeof(*temp) * size);
	p_temp = temp;
	for (i = 0; i<b[0].players; i++)
		if (p[i].buy_amount>0)
			temp[j++]=p[i];
	printf("There are folowing request for buying raw units:\n");
	for (i = 0; i < size; i++) {
		am = temp[i].buy_amount;
		pr = temp[i].buy_price;
		printf("%10s wants to buy %10d units ",temp[i].name,am);
		printf("for %10d$ each\n",pr);
	}
	qsort(temp,size,sizeof(*temp),(int (*)(const void*,const void*))(cmp_buy));
	printf("Bank solves the queries according to the priority:\n");
	printf("Result:\n");
	for (i = 0; i < size; i++) {
		am = temp[i].buy_amount;
		pr = temp[i].buy_price;
		printf("%10d$ for each unit of %10s's query\n",pr,temp[i].name);
	}
	while (size && b[0].raw_amount) {
		sum = j = 0;
		max = temp[0].buy_price;
		for (i=0; i<size; i++) {
			if (temp[i].buy_price == max) {
				sum += temp[i].buy_amount;
				j++;
			}
		}
		if (sum <= b[0].raw_amount) {
			for (i=0; i<j; i++)
				comply_buy(b,p,temp[i].fd);
			size -= j;
			temp += j;
			b[0].raw_amount -= sum;
		} else {
			i = 0;
			sum = b[0].raw_amount;
			while (sum) {
				comply_buy_one(b,&sum,p,temp[i % j].fd);
				i++;
			}
			size = 0;
		}
	}
	free(p_temp);
}

void auction_sell(struct bankir *b, struct players *p)
{
	int i,j=0,size,am,pr,min,sum;
	struct players *temp,*p_temp;
	char buf[1024]="\0",num[10]="\0";

	size = sellers(b,p);
	if (!size) {
		printf("Auction: sell: no requests for selling product:\n");
		add_str(buf,"        BANK BOUGHT   amount      ");
		add_str(buf,"price     seller\n");
		add_str(buf,"%                ");
		itoa2(0,num);
		add_str(buf,num);
		itoa2(0,num);
		add_str(buf,num);
		add_str(buf,"     ");
		add_str(buf,"nobody\n");
		for(i=0;i<b[0].players;i++){
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
		}
		return;
	}
	add_str(buf,"        BANK BOUGHT   amount      price     seller\n");
	for(i=0;i<b[0].players;i++){
		if (0 >= write(p[i].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[i].fd);
	}
	temp = malloc(sizeof(*temp) * size);
	p_temp = temp;
	for (i = 0; i<b[0].players; i++)
		if (p[i].sell_amount>0)
			temp[j++]=p[i];
	printf("There are folowing request to sell production\n");
	for (i = 0; i < size; i++) {
		am = temp[i].sell_amount;
		pr = temp[i].sell_price;
		printf("%10s wants to sell %10d units ",temp[i].name,am);
		printf("for %10d$ each\n",pr);
	}
	qsort(temp,size,sizeof(*temp),(int (*)(const void*,const void*))(cmp_buy));
	printf("Bank solves the queries according to the priority:\n");
	printf("Result:\n");
	for (i = 0; i < size; i++) {
		am = temp[i].sell_amount;
		pr = temp[i].sell_price;
		printf("%10d$ for each unit of %10s's query\n",pr,temp[i].name);
	}
	while (size && b[0].product_amount) {
		sum = j = 0;
		min = temp[0].sell_price;
		for (i=0; i<size; i++) {
			if (temp[i].sell_price == min) {
				sum += temp[i].sell_amount;
				j++;
			}
		}
		if (sum <= b[0].product_amount) {
			for (i=0; i<j; i++)
				comply_sell(b,p,temp[i].fd);
			size -= j;
			temp += j;
			b[0].product_amount -= sum;
		} else {
			i = 0;
			sum = b[0].product_amount;
			while (sum) {
				comply_sell_one(b,&sum,p,temp[i % j].fd);
				i++;
			}
			size = 0;
		}
	}
	free(p_temp);
}


struct players *
connect_new_client(struct bankir *bank,struct players *player)
{
	int sfd,i;

	if (FD_ISSET(bank[0].ls, &bank[0].readfds)) {
		sfd = accept(bank[0].ls,0,0);
		if (bank[0].players < bank[0].max){
			player = add_player(player, &bank[0].players, sfd);
			print_new(sfd,bank[0].players);
			for (i = 0; i < bank[0].players; i++){
				welcome(player[i].fd, sfd);
			}
		} else {
			write_sorry(sfd);
			shutdown(sfd, 2);
		}
	}
	return player;
}

struct players *
work_with_players(struct bankir *bank,struct players *player)
{
	int i, rr, del = 0;
	
	for (i = 0; i < bank[0].players; i++){
		if (FD_ISSET(player[i].fd, &bank[0].readfds)) {
			rr = read_player(player,i);
			if (rr == 0) {
				shutdown(player[i].fd, 2);
				close(player[i].fd);
				del++;
				print_old(player[i].fd, bank[0].players - del);
				pre_del(player, bank[0].players, player[i].fd);
			} else {
				command(bank,player,i);
			}
		}
	}
	player=del_player(player,&bank[0].players,del);
	return player;
}

void level(struct bankir *bank)
{
	const int level_change[5][5] = {
		{ 4, 4, 2, 1, 1 },
		{ 3, 4, 3, 1, 1 },
		{ 1, 3, 4, 3, 1 },
		{ 1, 1, 3, 4, 3 },
		{ 1, 1, 2, 4, 4 }
	};
	int r,s,i;

	s = 0;
	r = 1 + (int)(12.0*rand()/(RAND_MAX+1.0));
	for (i = 0; s<r; i++)
			s += level_change[bank[0].level-1][i];
	bank[0].level = i;
}

void bank_update(struct bankir *bank,struct players *player)
{
	int p,l;
	float k;

	p = active_players(bank,player);
	l = bank[0].level;
	k = 0.5 * (l+1);
	bank[0].raw_amount = p*k;
	bank[0].raw_price = 800 - l*100 + ((l==1)?100:0) - ((l==2)?50:0);
	bank[0].product_amount = p*(4.0-k);
	bank[0].product_price = 6500 - (l-1)*500;
}

void print_all(struct bankir *bank, struct players *p)
{
	int i,j; 
	printf("User INFORMATION\n");
	printf("NAME      MONEY     PRODUCTS  RAW       BUILDINGS\n");
	for (i = 0; i< bank[0].players; i++) {
		printf("%-10s%-10d",p[i].name,p[i].money);
		printf("%-10d%-10d",p[i].product,p[i].raw);
		for (j=5; j>=1; j--) {
			if (p[i].buildings[j]!= 0) {
				printf("%d",p[i].buildings[j]);
				printf("(%d month left) ",j);
			}
		}
		printf("\n");
	}
}

void before_start(struct bankir *bank, struct players *p)
{
	if (bank[0].players < bank[0].max || bank[0].start)
		return;
	
	char buf[1024]="\0";
	int wc,i,ra,rp,pa,pp;

	bank_update(bank,p);
	bank[0].start = 1;
	printf("\nMonth %d. Level %d.\n",bank[0].month,bank[0].level);
	ra = bank[0].raw_amount;
	rp = bank[0].raw_price;
	pa = bank[0].product_amount;
	pp = bank[0].product_price;
	printf("  Bank sells %10d raw units for %10d.\n",ra,rp);
	printf("  Buys       %10d raw units for %10d.\n",pa,pp);
	print_all(bank,p);
	add_str(buf,"The game has been started!\n");
	add_str(buf,"Please enter your name:\n>");
	for (i = 0; i < bank[0].players; i++){
		wc = write(p[i].fd, buf, strlen(buf) + 1);
		if (wc == -1)
			error_detected("write");
	}
}

int all_turn(struct bankir *b, struct players *p)
{
	int i,s=1;
	for (i=0; i<b[0].players; i++)
		s &= p[i].turn;
	return s*i;
}

void turn_new(struct bankir *b, struct players *p)
{
	int i;

	for (i=0; i<b[0].players; i++)
		if (p[i].state > 0){
			p[i].turn = 0;
			p[i].buy_price = 0;
			p[i].buy_amount = 0;
			p[i].sell_price = 0;
			p[i].sell_amount = 0;
		}
}

void costs(struct bankir *b, struct players *p)
{
	int i,f,pr,r;

	for (i = 0; i<b[0].players; i++) {
		f = p[i].factory;
		pr = p[i].product;
		r = p[i].raw;
		if (p[i].state > 0)
			p[i].money = p[i].money - 300*r - 500*pr - 1000*f;
	}
}

void production(struct bankir *b, struct players *p)
{
	int i;
	for (i = 0; i<b[0].players; i++)
       		if (p[i].state > 0){
			p[i].product += p[i].product_to_make;
			p[i].raw -= p[i].product_to_make;
			p[i].product_to_make = 0;
		}
}

void lost(struct players *p,int i)
{
	char buf[1024]="\0";
	add_str(buf,"\n");
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	add_str(buf,"|                    L                      |\n");
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}

void won(struct players *p,int i)
{
	char buf[1024]="\0";
	add_str(buf,"\n");
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	add_str(buf,"|             W                             |\n");
	add_str(buf,"|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n");
	if (0 >= write(p[i].fd,buf,strlen(buf)+1))
		printf("write error to fd = %d",p[i].fd);
}


void bankruptcy_control(struct bankir *b, struct players *p)
{
	if (active_players(b,p) == 1)
		return;
	int i;
	for (i=0; i<b[0].players; i++)
		if (p[i].money < 0 && p[i].state != -1){
			p[i].state = -1;
			lost(p,i);
			printf("%s has become bankrupt.\n",p[i].name);
		}
}

void winner_control(struct bankir *b, struct players *p)
{
	if (active_players(b,p) != 1)
		return;
	int i;
	for (i = 0; i<b[0].players; i++)
		if (p[i].state > 1) {
			p[i].state = -2;
			won(p,i);
			printf("%s has won the game.\n",p[i].name);
		}
}

void information(struct bankir *b, struct players *p)
{
	char buf[1024]="\0";
	int j;
	add_str(buf,"All players completed their turn.\n");
	add_str(buf,"\nAUCTION results:\n");
	for(j=0;j<b[0].players;j++){
		if (0 >= write(p[j].fd,buf,strlen(buf)+1))
			printf("write error to fd = %d",p[j].fd);
	}
}

void construction (struct bankir *b, struct players *p)
{
	int i,j,n;
	char *s, buf[1024]="\0", num[10]="\0";

	for (i = 0; i<b[0].players; i++) {
		for (j = 1; j<=5; j++)
			p[i].buildings[j-1] = p[i].buildings[j];
		p[i].buildings[5]=0;
		if (p[i].buildings[0] != 0) {
			s = p[i].name;
			n = p[i].buildings[0];
			if (n == 1)
				printf("%s gets %d new factory.\n",s,n);
			else
				printf("%s gets %d new factories.\n",s,n);
			add_str(buf,"Now you have ");
			itoa(n,num);
			add_str(buf,num);
			add_str(buf," more factories.\n");
			if (0 >= write(p[i].fd,buf,strlen(buf)+1))
				printf("write error to fd = %d",p[i].fd);
			p[i].factory += n;
			p[i].money -= n*2500;
			p[i].buildings[0] = 0;
		}
	}
}

int main(int argc,char **argv)
{
	struct players *player;
	struct bankir bank = {0,3,1,0,0,0};

	bank.max = atoi(argv[1]);
	printf("Server is ready. Maximum number of players is %d\n",bank.max);
	bank.ls = start_listen(atoi(argv[2]));
	for (;;) {
		if (all_turn(&bank,player) && active_players(&bank,player)){
			information(&bank,player);
			production(&bank,player);
			auction_buy(&bank,player);
			auction_sell(&bank,player);
			construction(&bank,player);
			costs(&bank,player);
			bank.month += 1;
			level(&bank);
			bank_update(&bank,player);
			printf("\nMonth %d. Level %d.\n",bank.month,bank.level);
			print_all(&bank,player);
			new_month_notification(&bank,player);
			bankruptcy_control(&bank,player);
			turn_new(&bank,player);
			winner_control(&bank,player);
			if (!active_players(&bank,player))
				printf("****GAME OVER****\n");
		}
		before_start(&bank,player);
		my_select(&bank,player);
		player = connect_new_client(&bank,player);
		player = work_with_players(&bank,player);
	}
	return 0;
}
