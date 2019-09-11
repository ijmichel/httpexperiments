/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <regex.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 999999 // max number of bytes we can get at once


struct uriInfo {
    char  *protocol;
    char  *server;
    char  *serverPort;
    char  *fullPathWithoutPort;
    char  *port;
    char  *path;
};

struct httpResponse {
    char  *httpStatusCd;
    char  *body;
};

struct uriInfo *getUriDetails(char str[], struct uriInfo *iUriInfo);
struct httpResponse *parseResponse(char* buf, struct httpResponse *httpResponseDtl);

void writeMessageToFile(const char *message);

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

    struct uriInfo *clientUriInfo = (struct uriInfo *) malloc(sizeof(struct uriInfo));
    clientUriInfo = getUriDetails(argv[1],clientUriInfo);

    if(strcmp(clientUriInfo->protocol,"http:") != 0){
        writeMessageToFile("INVALIDPROTOCOL");
        fprintf(stderr, "INVALIDPROTOCOL");
        return(1);
    }

	if ((rv = getaddrinfo(clientUriInfo->server,clientUriInfo->port,&hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        writeMessageToFile("NOCONNECTION");
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
            writeMessageToFile("NOCONNECTION");
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
            writeMessageToFile("NOCONNECTION");
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
        writeMessageToFile("NOCONNECTION");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	char msg [100];
    sprintf(msg,"GET %s HTTP/1.0\r\nUser-Agent: Wget/1.15 (linux-gnu)\r\nAccept: */*\nHost: %s\r\nConnection: Keep-Alive\n\n",clientUriInfo->path,clientUriInfo->serverPort);
   // sprintf(msg,"GET %s ","test");
	int len, bytes_sent;
	len = strlen(msg);

    if( bytes_sent = send(sockfd,msg,len,0) < 0)
    {
        puts("Send failed");
        return 1;
    }


    if((numbytes = recv(sockfd, buf,MAXDATASIZE,0)) == -1){
	    perror("recv");
        writeMessageToFile("NOCONNECTION");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

    writeMessageToFile(buf);

    struct httpResponse *httpResponseDtl = (struct httpResponse *) malloc(sizeof(struct httpResponse));
    httpResponseDtl = parseResponse(buf,httpResponseDtl);

    if(strcmp(httpResponseDtl->httpStatusCd,"HTTP/1.0 404 Not Found") == 0){
        writeMessageToFile("FILENOTFOUND");
    }else{
        writeMessageToFile(httpResponseDtl->body);
    }

	close(sockfd);

	return 0;
}

struct httpResponse *parseResponse(char* buf, struct httpResponse *httpResponseDtl){
    char* body = strstr(buf, "\r\n\r\n") + 4;
    char* header = 0;
    if(body) {
        // allocate memory to hold the entire header
        header = malloc((body - buf) + 1);
        if(header) {
            // copy header to buffer
            memcpy(header, buf, body - buf);
            // null terminate the buffer
            header[body - buf] = 0;
            // do something with the buffer

        }
    }

    if(body != NULL){
        httpResponseDtl->body = body;
    }

    char* httpStat;
    httpStat = strtok(buf, "\r\n");
    //"HTTP/1.0 200 OK"
    if( httpStat != NULL ){
        httpResponseDtl->httpStatusCd = httpStat;
    }

//    token = strtok(NULL, "\r\n");
//    while( token != NULL ) {
//        if (strstr(token, "Content-Length") != NULL) {
//            int length;
//            sscanf(token,"Content-Length: %d", &length);
//        }
//        token = strtok(NULL, "\r\n");
//    }

}

void writeMessageToFile(const char *message) {
    FILE *fp;
    fp = fopen("output", "w+");
    fprintf(fp, "%s",message);
    fclose(fp);
}

struct uriInfo *getUriDetails(char str[], struct uriInfo *iUriInfo){
    char *token;

    /* get the first token */
    token = strtok(str, "//");

    /* walk through other tokens */
    int count = 0;
    while( token != NULL ) {
        if(count == 0) {
              iUriInfo->protocol = token;
              token = strtok(NULL, "/");
        }
        else if(count == 1){
             iUriInfo->server = token;
             token = strtok(NULL, "\0");
        }
        else {
           iUriInfo->path = token;
            token = strtok(NULL, "\0");
        }
        count++;
    }

    //Parse the port from the server if its there
    if(iUriInfo->server != NULL){
        char *port;
        port = strtok(iUriInfo->server, ":");
        iUriInfo->server = port;
        if(port != NULL){
            port = strtok(NULL, ":");
            if(port != NULL){
                iUriInfo->port = port;
            }else{
                iUriInfo->port = "80";
            }

        }
    }

    if(iUriInfo->protocol != NULL && iUriInfo->server != NULL){
        int fullPathSize = strlen(iUriInfo->protocol)  + strlen("//") + strlen(iUriInfo->server) + strlen("/") + 1;
        if(iUriInfo->path != NULL){
            fullPathSize += strlen(iUriInfo->path);
        }

        char * newBuffer = (char *)malloc(fullPathSize);
        strcpy(newBuffer,iUriInfo->protocol);
        strcat(newBuffer,"//");
        strcat(newBuffer,iUriInfo->server);
        strcat(newBuffer,"/");
        if(iUriInfo->path != NULL) {
            strcat(newBuffer, iUriInfo->path);
        }
        iUriInfo->fullPathWithoutPort = newBuffer;
    }

    if(iUriInfo->port != NULL && iUriInfo->server != NULL){
        int fullPathSize = strlen(iUriInfo->protocol)  + strlen("//") + strlen(iUriInfo->server) + strlen(":") + strlen(iUriInfo->port) + 1 ;

        char * newBuffer = (char *)malloc(fullPathSize);
        strcpy(newBuffer,iUriInfo->protocol);
        strcat(newBuffer,"//");
        strcat(newBuffer,iUriInfo->server);
        strcat(newBuffer,":");
        strcat(newBuffer, iUriInfo->port);

        iUriInfo->serverPort = newBuffer;
    }

    if(iUriInfo->path != NULL ){
        int fullPathSize = strlen("/") + strlen(iUriInfo->path) + 1;
        char * newBuffer = (char *)malloc(fullPathSize);
        strcpy(newBuffer,"/");
        strcat(newBuffer,iUriInfo->path);

        iUriInfo->path = newBuffer;
    }



    return iUriInfo;
}

