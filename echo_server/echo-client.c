#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MYPORT 4000
#define MAXS 100

void errxt(char *s);

int main(int argc, char **argv)
{
	int sockfd;
	int numbytes;
	char buf[MAXS];
	char array[MAXS];
	struct hostent *he;
	struct sockaddr_in tr;

	if (argc != 2) {
		fprintf(stderr,"usage : client hostname\n");
		exit(EXIT_FAILURE);
	}

	if ((he = gethostbyname(argv[1])) == NULL) {
		errxt("gethostbyname");
	}

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		errxt("socket");
	}
	memset(&(tr.sin_zero),0, sizeof tr);
	tr.sin_family = AF_INET;
	tr.sin_port = htons(MYPORT);
	tr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd,(struct sockaddr *)&tr,sizeof(tr))== -1) {
		errxt("server connection");
	}
	for(;;) {
		printf("message: ");
		fgets(array ,MAXS, stdin);
		if (send(sockfd, array, strlen(array), 0) == -1)
			perror("send");

		if ((numbytes = recv(sockfd, buf, MAXS-1, 0)) == -1) {
			errxt("recv");
		}
		buf[numbytes] = '\0';

		printf("echo message: %s\n", buf);
	}
	close(sockfd);
	return 0;
}

void errxt(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}
