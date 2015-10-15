#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

void error_detected(const char * s)
{
	perror(s);
	exit(1);
}

void do_command(char *input,int fd)
{
	int wc;
	wc = write(fd,input,strlen(input)+1);
	if (wc <= 0)
		printf("write error to fd = %d",fd);
}

int *add_sfd(int *old_arr, int *arr_size, int new_sfd)
{

	int *temp_arr,i;
	temp_arr = malloc(sizeof(int) * (*arr_size + 1));
	for (i = 0; i < *arr_size; i++)
		temp_arr[i]  = old_arr[i];
	temp_arr[i] = new_sfd;
	(*arr_size) += 1;
	free(old_arr);
	return temp_arr;
}

int *del_sfd(int *old_arr, int *arr_size, int old_sfd, int num_to_del)
{
	int *temp_arr,i,j=0;
	
	temp_arr = malloc(sizeof(int) * (*arr_size - num_to_del));
	for (i = 0; i < *arr_size; i++){
		if (old_arr[i] != old_sfd)
			temp_arr[j++] = old_arr[i];
	}
	free(old_arr);
	*arr_size -= num_to_del;
	return temp_arr;
}

int main(int argc,char **argv)
{
	int ls,port,sfd,i,*sfd_arr=NULL,sfd_size=0,rr;
	struct sockaddr_in addr;	
	char buf[1024];

	port = atoi(argv[1]);
	ls = socket(AF_INET,SOCK_STREAM, 0);
	if (ls == -1)
		error_detected("ls");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	int opt = 1;
	setsockopt(ls,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (0 != bind(ls, (struct sockaddr *) &addr, sizeof(addr)))
		error_detected("bind");
	if (-1 == listen(ls,5))
		error_detected("listen");
	for (;;) {
		fd_set readfds;
		int max_d = ls;
		FD_ZERO(&readfds);
		FD_SET(ls, &readfds);
		for (i = 0; i < sfd_size ; i++) {
			FD_SET(sfd_arr[i], &readfds);
			if (sfd_arr[i] > max_d)
				max_d = sfd_arr[i];
		}
		int res = select(max_d + 1, &readfds, NULL, NULL, NULL);
		if (res < 1) {
			if ( errno != EINTR)
				error_detected("select");
			else
				printf("signal");
			continue;
		}
		if (FD_ISSET(ls, &readfds)) {
			sfd = accept(ls,0,0);
			sfd_arr = add_sfd(sfd_arr, &sfd_size, sfd);
		}
		for (i = 0; i < sfd_size; i++){
			if (FD_ISSET(sfd_arr[i], &readfds)) {
				rr = read(sfd_arr[i], buf, sizeof(buf) - 1);
				if (rr == -1)
					error_detected("read");
				if (rr == 0) {
					shutdown(sfd_arr[i],2);
					close(sfd_arr[i]);
				} else {
					buf[rr] = '\0';
					do_command(buf,i);
				}
			}
		}
	}
	return 0;
}
