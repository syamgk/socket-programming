/* talker.c -- a datagram "client" demo */
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

#define MAXBUFLEN 140
#define SERVERPORT "4950"	// the port users will be connecting to

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char msg[MAXBUFLEN];
	char s[INET6_ADDRSTRLEN];
	socklen_t addlen;
	if (argc != 2) {
		fprintf(stderr,"usage: talker hostname\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	freeaddrinfo(servinfo);
	addlen = sizeof their_addr;
	while(1) {
		printf("Me <entr_txt>:");
		gets(msg);         // read msg
		if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
				 p->ai_addr, p->ai_addrlen)) == -1) {           // send msg
			perror("talker: sendto");
			exit(1);
		}
		if ((numbytes = recvfrom(sockfd, msg, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addlen)) == -1) { // receive msg
			perror("recvfrom");
			exit(1);
		}	
		msg[numbytes] = '\0';
		printf("\t\t\t\t\tyou<%s> : \"%s\"\n", inet_ntop(their_addr.ss_family,
			  get_in_addr((struct sockaddr *)&their_addr),
			  s, sizeof s), msg); // print incoming msg
	}
	close(sockfd);
	return 0;
}
