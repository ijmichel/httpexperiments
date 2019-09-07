/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stddef.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 


struct uriInfo {
    char  *protocol;
    char  *server;
    char  *serverPort;
    int   port;
    char  *path;
};

struct uriInfo *getUriDetails(char str[], struct uriInfo *iUriInfo);

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
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

    char str[] = "http://192.168.0.2:8888/servlet/rece";

    struct uriInfo *clientUriInfo = malloc(sizeof(struct uriInfo));
    clientUriInfo = getUriDetails(str,clientUriInfo);

    printf(clientUriInfo->server);

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}

struct uriInfo *getUriDetails(char str[], struct uriInfo *iUriInfo){
    char *token;

    /* get the first token */
    token = strtok(str, "/");

    /* walk through other tokens */
    int count = 0;
    while( token != NULL ) {
        if(count == 0) {
              iUriInfo->protocol = token;
              token = strtok(NULL, "/");
        }
        else if(count == 1){
             iUriInfo->serverPort = token;
             token = strtok(NULL, "\0");
        }
        else {
           iUriInfo->path = token;
            token = strtok(NULL, "\0");
        }
        count++;
    }

    if(iUriInfo->serverPort != NULL){
        char *port;
        port = strtok(iUriInfo->serverPort, ":");
        if(port != NULL){
            port = strtok(NULL, ":");
            iUriInfo->port = atoi(port);
        }
    }

    return iUriInfo;
}

