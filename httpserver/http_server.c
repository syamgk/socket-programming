/* http_server "server " */
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
#include <sys/stat.h>
#include <fcntl.h>

#define FROM_PORT 4321	// the port users will be connecting to
#define MAXBUFLEN 500
#define BACKLOG 10 // no of concurrent access 
// parse the header and send corresponding data
void parse_and_reply(char*, int);
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
// forever loop
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
			parse_and_reply(buf, new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;
}

void parse_and_reply(char *buf, int new_fd) 
{
// parsing header
	int fd;
	char buffr[100];
	char *words[3];
	char *cwd, *wd; // current workind dir
	words[0] = strtok(buf , " ");	// GET
	words[1] = strtok(NULL, " ");	// '/'
	words[2] = strtok(NULL, "/\n ");// HTTP/1.1 
	//	printf(">>>>>>>>>>%s\n",words[0]);
	if(strcmp(words[0],"GET")) 
		printf("Error\n");
	if(!strcmp(words[1],"/")) 
		words[1] = "/index.html";
	if(strcmp(words[2],"HTTP")) 
		printf("Error\n");
	printf(">>>>>>>>>>%s\n",words[1]);
	cwd = getenv("PWD");
	wd = strdup(cwd);
	wd = strcat(wd,words[1]);
//	printf("opening . . .\n%s", wd);
	if((fd = open( wd, O_RDONLY)) == -1 ) {
	//	perror("open");
		exit(0);
	}
	while(read(fd, buffr, 1)) {
		write(new_fd, buffr, 1);
	}
	close(fd);
	close(new_fd);
}
