#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "a1_lib.h"
#include <unistd.h>

#define BUFSIZE   1024

int main(int argc, char *argv[]) {
    message  msg;
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    int sockfd;
    char user_input[BUFSIZE] = { 0 };
    char server_msg[BUFSIZE] = { 0 };
    if (connect_to_server(argv[1], atoi(argv[2]), &sockfd) < 0) {
        perror("client socket create error\n");
        return EXIT_FAILURE;
    }
    
    while (1) {
        memset(msg.input, 0, sizeof(msg.input));
        fflush(stdin);
        memset(user_input, 0, sizeof(user_input));
        memset(server_msg, 0, sizeof(server_msg));
        
        // read user input from command line
        printf(">> ");
        fgets(user_input, BUFSIZE, stdin);
        strcpy(msg.input, user_input);
        //receive a msg from the server. we will send everything to the server and it will process it
        send_message(sockfd, (char *)&msg.input, sizeof(msg));
        if(strcmp(user_input, "exit\n")==0){
            break;
        }
        if(strcmp(user_input, "shutdown\n")==0){
            printf("Bye!\n");
            break;
        }
        
        ssize_t byteCount = recv_message(sockfd, server_msg, sizeof(server_msg));
        if (byteCount <= 0) {
            break;
        }
        printf("%s\n", server_msg);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}