#include <stdio.h>
#include <string.h>
# include <time.h>
#include "a1_lib.h"
#include <unistd.h>

#define BUFSIZE   1024

int main(void) {
    int sockfd, clientfd;
    char msg[BUFSIZE];
    const char *greeting = "hello, world";
    int running = 1;
    
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
    
    while (strcmp(msg, "quit\n")) {
        memset(msg, 0, sizeof(msg));
//        ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
//        if (byte_count > 0) {
//            printf("Client: %s\n", msg);
//        }
        int simulateRead = rand() % 10 + 1;
        printf("reading for %d\n", simulateRead);
        send_message(clientfd, greeting, strlen(greeting));
        sleep(2);
    }
    
    return 0;
}

