#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>


int main(int argc, char* argv[]) {

    if (argc!=2) {
    
        fprintf(stderr, "Format : %s <port no>i\n", argv[0]);
        exit(1);
    }


    struct addrinfo *hint, *res;
    hint = malloc(sizeof(struct addrinfo));
    bzero(&hint, sizeof(hint));

    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype=SOCK_STREAM;

    int getAiStatus;

    if((getAiStatus = getaddrinfo("localhost", argv[1], hint, res)) < 0 ) {
            fprintf(stderr, "getaddrinfo error : %s\n", gai_strerror(getAiStatus));
            exit(1);
    }

    int sockFd;

    if((sockFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }

    if(bind(sockFd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        exit(1);
    }

    if(listen(sockFd, BACKLOG) < 0) {
        perror("listen");
        exit(1);
    }

    struct sockaddr_storage cliAddr;
    int clilen = sizeof(cliAddr);

    while(1) {
        
        int cliFd;
        if((cliFd = accept(sockFd, (struct sockaddr *)&cliAddr, &clilen)) < 0) {
            perror("Accept");
            exit(1);
        }
 
       pthread_t clientHandler;
       pthread_attr_t attr;
       pthread_attr_init(&attr);

        if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) !=0) {
            perror("pthread_attr_setdetachedstate");
            exit(1);
    }   

        if(pthread_create(&clientHandler, &attr, handleClient, (void *) cliFd)!=0) {
            perror("pthread_create");
            exit(1);
        }
    }
    
    close(sockFd);
    return 0;

}


