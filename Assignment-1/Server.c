#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "a1_lib.h"

#define BUFSIZE   1024
const char kEndOfTransfer = 26;

int addInts(int x, int y) {
    return x + y;
}

int multiplyInts(int a, int b) {
    return a * b;
}

float divideFloats(float a, float b) {
    return a / b;
}

void sleeps(int x) {
    wait(&x);
}

uint64_t factHelper(int cur, uint64_t acc) {
    if (cur <= 1) {
        return acc;
    } else {
        return factHelper(cur - 1, acc * cur);
    }
}

uint64_t factorial(int x) {
    return factHelper(x, 1);
}


int isCommandValid(char *cmd) {
    char *commands[] = {"add", "multiply", "divide", "sleep", "factorial", "exit\n"};
    int len = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < len; ++i) {
        if (strcmp(commands[i], cmd) == 0) {
            return i + 1;
        }
    }
    return 0;
}

int isInputLengthValid(char *cmd, int expectedLength) {
    cmd = strtok(NULL, " ");
    int curr = 0;
    while (cmd != NULL) {
        cmd = strtok(NULL, " ");
        curr++;
    }
    return curr == expectedLength;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    
    int sockFd, clientFd;
    char msg[BUFSIZE];
    char *returnMsg;
    if (create_server("127.0.0.1", atoi(argv[1]), &sockFd) < 0) {
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

//        int pid = fork();
//
//        if(pid < 0) {
//            return EXIT_FAILURE;
//        } else if( pid ==0){
//
//            dup2(clientFd, fileno(stdout));
//            // execute command
//            system(msg);
//            // indicate end of command output
//            putc(kEndOfTransfer, stdout);
//            fflush(stdout);
//            exit(EXIT_SUCCESS);
//        } else{
//            wait(NULL);
//        }
        
        char *token = strtok(msg, " ");
        int result = isCommandValid(token);
        if (result == 1) {
            if (isInputLengthValid(msg, 2)) {
                returnMsg = "valid ADD";
            } else {
                returnMsg = "not valid ADD";
            }
        } else if (result == 2) {
            if (isInputLengthValid(msg, 2)) {
                returnMsg = "valid multiply";
            } else {
                returnMsg = "not valid multiply";
            }
        } else if (result == 3) {
            if (isInputLengthValid(msg, 2)) {
                returnMsg = "valid divide";
            } else {
                returnMsg = "not valid divide";
            }
        } else if (result == 4) {
            if (isInputLengthValid(msg, 1)) {
                returnMsg = "valid sleep";
            } else {
                returnMsg = "not valid sleep";
            }
        } else if (result == 5) {
            if (isInputLengthValid(msg, 1)) {
                returnMsg = "valid factorial";
            } else {
                returnMsg = "not valid factorial";
            }
        } else if (result == 6) {
            returnMsg = "valid exit\n";
        } else {
            returnMsg = "not valid";
        }
        
        if (byteCount <= 0) {
            break;
        }
        send_message(clientFd, returnMsg, strlen(returnMsg));
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}