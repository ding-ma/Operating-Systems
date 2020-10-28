#include <stdio.h>
#include <string.h>
# include <time.h>
#include "socket_lib.h"
#include <unistd.h>

#define BUFSIZE   1024

int main(void) {
    int sockfd, clientfd;
    char msg[BUFSIZE];
    char greeting[BUFSIZE];
    
    if (create_server("0.0.0.0", 6969, &sockfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    
    if (accept_connection(sockfd, &clientfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    srand(time(NULL)); //used to simulate read()
    printf("connection accepted \n");
    
    int i = 0;
    while (strcmp(msg, "quit\n")) {
        memset(msg, 0, sizeof(msg));
        ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
        if (byte_count > 0) {
            printf("Client: %s\n", msg);
        }
        
        int simulateRead = rand() % 10 + 1;
        sprintf(greeting, "hello world from server %d", i);
        printf("blocking read for %d\n", simulateRead);
        send_message(clientfd, greeting, strlen(greeting));
        sleep(simulateRead);
        i++;
        memset(greeting, 0, sizeof(greeting));
        
    }
    
    return 0;
}

