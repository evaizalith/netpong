// Contains functions for opening and connecting to a server

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include "network.h"

#define HOSTLEN 256

int makeServerSocket(int portnum, int backlog) {
    struct sockaddr_in saddr; 
    struct hostent *hp;
    char hostname[HOSTLEN];
    int sock_id;

    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    
    if (sock_id == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    bzero((void*)&saddr, sizeof(saddr));

    gethostname(hostname, HOSTLEN);
    hp = gethostbyname(hostname);

    bcopy((void*)hp->h_addr, (void*)&saddr.sin_addr, hp->h_length);
    saddr.sin_port = htons(portnum);
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;

    if (bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0) {
        fprintf(stderr, "Failed to bind socket\n");
        return -1;
    }

    if (listen(sock_id, backlog) != 0) {
        fprintf(stderr, "Failed to listen\n");
        return -1;
    }

    return sock_id;
}

int connectToServer(char *host, int portnum) {
    int sock;
    struct sockaddr_in servadd;
    struct hostent *hp;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "Socket is null\n");
        return -1;
    }

    bzero(&servadd, sizeof(servadd));
    hp = gethostbyname(host);

    if (hp == NULL) {
        fprintf(stderr, "Host is null\n");
        return -1;
    }

    bcopy(hp->h_addr, (struct sockaddr*)&servadd.sin_addr, hp->h_length);

    servadd.sin_port = htons(portnum);
    servadd.sin_family = AF_INET;

    if (connect(sock, (struct sockaddr*)&servadd, sizeof(servadd)) != 0) {
        perror("Failed to connect to socket");
        return -1;
    }

    return sock;
}


