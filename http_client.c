/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>


#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 30000 // max number of bytes we can get at once


struct uriInfo {
    char  *protocol;
    char  *server;
    char  *serverPort;
    char  *fullPathWithoutPort;
    char  *port;
    char  *path;
};

struct httpResponse {
    char *httpStatusCd;
    int  contentLength;
    char* header;
    char *body;
};

struct uriInfo *getUriDetails(char str[], struct uriInfo *iUriInfo);
struct httpResponse *parseResponse(char* buf, struct httpResponse *httpResponseDtl);

void writeMessageToFile(const char *message);

void writeBinaryFile(const char *message);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct uriInfo *clientUriInfo = (struct uriInfo *) malloc(sizeof(struct uriInfo));
    clientUriInfo = getUriDetails(argv[1], clientUriInfo);

    if (strcmp(clientUriInfo->protocol, "http:") != 0) {
        writeMessageToFile("INVALIDPROTOCOL");
        fprintf(stderr, "INVALIDPROTOCOL");
        return (1);
    }

    if ((rv = getaddrinfo(clientUriInfo->server, clientUriInfo->port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        writeMessageToFile("NOCONNECTION");
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
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

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    char msg[100];
    sprintf(msg,
            "GET %s HTTP/1.1\r\nUser-Agent: Wget/1.15 (linux-gnu)\r\nAccept: */*\nHost: %s\r\nConnection: Keep-Alive\n\n",
            clientUriInfo->path, clientUriInfo->serverPort);
    // sprintf(msg,"GET %s ","test");
    int len, bytes_sent;
    len = strlen(msg);

    if (bytes_sent = send(sockfd, msg, len, 0) < 0) {
        puts("Send failed");
        return 1;
    }

    FILE *fp;
    fp = fopen("output", "wb"); //leave open for writing a binary file

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        writeMessageToFile("NOCONNECTION");
        exit(1);
    }

    printf("client: received '%s'\n", buf);

    struct httpResponse *httpResponseDtl = (struct httpResponse *) malloc(sizeof(struct httpResponse));
    httpResponseDtl = parseResponse(buf, httpResponseDtl);

    if (strstr(httpResponseDtl->httpStatusCd, "404") != NULL) {
        writeMessageToFile("FILENOTFOUND");
        return 0;
    }

    //Write the body only
    if (httpResponseDtl->body != buf) {
        int headerSize = (int) (httpResponseDtl->body - buf);
        //buf+headerSize+4 means pointer to where buf is moved header size over plus 4
        //The plus 4 is the CLRF
        fwrite(buf + headerSize + 4, sizeof(char), numbytes - headerSize - 4, fp);
        //numByes-headerSizs-4 is the size of the body really
    } else {//This might not happen but just in case
        fwrite(httpResponseDtl->body, sizeof(char), numbytes, fp);
    }
    memset(buf, '\0', sizeof buf);

    do {//Keep receiving the remainder of the data and writing it to the file (NOTE: there is no header here on subsequent writes)
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
            writeMessageToFile("NOCONNECTION");
            perror("recv");
            exit(1);
        }
        printf("client: received '%s'\n", buf);

        if (numbytes > 0) {

            if (buf != NULL) {
                fwrite(buf, sizeof(char), numbytes, fp);
            }
        }

    } while (numbytes > 0);

    buf[numbytes] = '\0';
    fclose(fp);

    close(sockfd);

    return 0;
}

struct httpResponse *parseResponse(char* buf, struct httpResponse *httpResponseDtl){
    char* body = strstr(buf, "\r\n\r\n");
    char* header = 0;
    if(body) {
        header = (char *)malloc((body - buf) + 1);
        if(header) {
            httpResponseDtl->header = header;
            memcpy(header, buf, body - buf);
            header[body - buf] = 0;
        }
    }

    if(body != NULL){
        httpResponseDtl->body = body;
    }

    char* headerItem;
    headerItem = strtok(header, "\r\n");
    //"HTTP/1.0 200 OK"
    int count = 0;
    while(headerItem != NULL){
        if( headerItem != NULL ){
            if(count == 0){//First one always HTTP
                httpResponseDtl->httpStatusCd = headerItem;
            }

            if(count > 0){
                if(strstr(headerItem,"Content-Length") != NULL){
                    int contentLength;
                    char tmp[256];
                    tmp[0]='\0';
                    while (sscanf(headerItem,"%[^0123456789]%s",tmp,headerItem)>1||sscanf(headerItem,"%d%s",&contentLength,headerItem))
                    {
                        if (tmp[0]=='\0')
                        {
                            httpResponseDtl->contentLength = contentLength;
                            break;
                        }
                        tmp[0]='\0';
                    }
                }
            }
            count++;
            headerItem = strtok(NULL, "\r\n");
        }
    }

    return httpResponseDtl;


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

