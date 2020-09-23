#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "a1_lib.h"
#include <sys/wait.h>

#define BUFSIZE   1024

int main(void) {
    int sockfd, clientfd;
    char msg[BUFSIZE];
    const char *greeting = "hello, world\n";
    int running = 1;
    
    if (create_server("127.0.0.1", 10000, &sockfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    
   
    while (strcmp(msg, "quit\n")) {
        int socket = accept_connection(sockfd, &clientfd);
        if ( socket< 0) {
            fprintf(stderr, "oh no\n");
            return -1;
        }
        
        if(!fork()){
            close(sockfd);
            while(1){
                memset(msg, 0, sizeof(msg));
                ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
                if (byte_count <= 0) {
                    break;
                }
                printf("Client: %s\n", msg);
                send_message(clientfd, greeting, strlen(greeting));
            }
        }
        close(socket);
    }
    
    return 0;
}

