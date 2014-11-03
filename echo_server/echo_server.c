/* echo_server "server " */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define FROM_PORT 4000	// the port users will be connecting to
#define MAXBUFLEN 140
#define BACKLOG 10

int main(int argc,char** argv)
{
	int sockfd, new_fd;
	int numbytes;
	struct sockaddr_in frm_addr, to_addr;
	char buf[MAXBUFLEN];
	int addr_len;
	int yes =1;
// set memory to 0
	memset(&frm_addr, 0, sizeof frm_addr);
	memset(&to_addr, 0, sizeof to_addr);
// from 
	frm_addr.sin_family = AF_INET; // set to AF_INET to force IPv4
	frm_addr.sin_port = htons(FROM_PORT);
	frm_addr.sin_addr.s_addr = INADDR_ANY; // use my IP
// create a socket fd
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
// bind frm_addr with sockfd
	if (bind(sockfd,(struct sockaddr*)&frm_addr, sizeof(struct sockaddr)) == -1) {
		close(sockfd);
		perror("bind");
		exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	addr_len = sizeof(struct sockaddr_in);
// reset fdset and add required file_descripters
	while(1) {
		printf("...........\n");
		if(( new_fd = accept( sockfd,(struct sockaddr *) &to_addr, &addr_len)) == -1) {
			perror("accept");
			exit(1);
		}
		printf("server : got incoming connection from %s\n",inet_ntoa(to_addr.sin_addr));
		if (fork() == 0 ) {  // on_child
			close(sockfd);
			if ((numbytes = recv(new_fd, buf, MAXBUFLEN-1, 0)) == -1) { 
				perror("recv");
				exit(1);
			}
			buf[numbytes] = '\0';
			printf("%s\n", buf);
			if(send(new_fd, buf, numbytes, 0) == -1) {
				perror("listner: sendto");
				exit(1);
			}
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;
}
