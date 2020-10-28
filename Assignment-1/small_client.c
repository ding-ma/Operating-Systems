#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "a1_lib.h"

#define BUFSIZE   1024

int main(void) {
    int sockfd;
    char user_input[BUFSIZE] = {0};
    char server_msg[BUFSIZE] = {0};
    
    if (connect_to_server("0.0.0.0", 6969, &sockfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    while (strcmp(user_input, "quit\n")) {
        memset(user_input, 0, sizeof(user_input));
        memset(server_msg, 0, sizeof(server_msg));
        
        // read user input from command line
        printf(">> ");
        fgets(user_input, BUFSIZE, stdin);
        // send the input to server
        send_message(sockfd, user_input, strlen(user_input));
        // receive a msg from the server
    }
    
    return 0;
}

