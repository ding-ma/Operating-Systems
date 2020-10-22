#include <stdio.h>
#include <string.h>

#include "a1_lib.h"

#define BUFSIZE   1024

int main(void) {
    int sockfd, clientfd;
    char msg[BUFSIZE];
    const char *greeting = "hello, world\n";
    int running = 1;
    
    if (create_server("0.0.0.0", 6969, &sockfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    
    if (accept_connection(sockfd, &clientfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    
    while (strcmp(msg, "quit\n")) {
        memset(msg, 0, sizeof(msg));
        ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
        if (byte_count > 0) {
            printf("Client: %s\n", msg);
            send_message(clientfd, greeting, strlen(greeting));
        }
    }
    
    return 0;
}

