#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "a1_lib.h"
#define BUFSIZE   1024
const char kEndOfTransfer = 26;

int addInts(int x, int y){
    return x+y;
}

int multiplyInts(int a, int b){
    return a*b;
}

float divideFloats(float a, float b){
    return a/b;
}

void sleeps(int x){
    wait(&x);
}

uint64_t factHelper(int cur, uint64_t acc){
    if (cur <= 1){
        return acc;
    } else{
        return factHelper(cur - 1, acc * cur);
    }
}

uint64_t factorial(int x){
    return factHelper(x, 1);
}


int isCommandValid(char* cmd){
    char *commands[] = {"add", "multiply", "divide", "sleep","factorial"};
    int len = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < len; ++i) {
        if (strcmp(commands[i], cmd) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    
    int sockFd, clientFd;
    char msg[BUFSIZE];
    const char *greeting = "hello, world\n";
    int running = 1;
    if (create_server(argv[1], atoi(argv[2]), &sockFd) < 0) {
        perror("listen socket create error\n");
        return EXIT_FAILURE;
    }
    if (accept_connection(sockFd, &clientFd) < 0) {
        perror("listen socket bind error\n");
        return EXIT_FAILURE;
    }

    while (strcmp(msg, "exit\n")) { //todo check if we want to put it here
        memset(msg, 0, sizeof(msg));
        ssize_t byteCount = recv_message(clientFd, msg, BUFSIZE);
        
        int pid = fork();
        
        if(pid < 0) {
            return EXIT_FAILURE;
        } else if( pid ==0){
    
            dup2(clientFd, fileno(stdout));
            // execute command
            system(msg);
            // indicate end of command output
            putc(kEndOfTransfer, stdout);
            fflush(stdout);
            exit(EXIT_SUCCESS);
        } else{
            wait(NULL);
        }
        
        char *token = strtok(msg, " ");
        if (isCommandValid(token)) { //check if the command is a valid one
        
        } else {
            printf("Wrong command \n");
        }
        
        if (byteCount <= 0) {
            break;
        }
        printf("Client: %s\n", msg);
        send_message(clientFd, greeting, strlen(greeting));
    }
    return EXIT_SUCCESS;
}