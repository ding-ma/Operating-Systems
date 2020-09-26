#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "a1_lib.h"

#define BUFSIZE   1024
const char kEndOfTransfer = 26;

int addInts(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    int x = atoi(token);
    token = strtok(NULL, " ");
    int y = atoi(token);
    
    return x + y;
}

int multiplyInts(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    int x = atoi(token);
    token = strtok(NULL, " ");
    int y = atoi(token);
    return x * y;
}

int isValidDivition(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    float x = atof(token);
    token = strtok(NULL, " ");
    float y = atof(token);
    
    return y != 0.0;
}

float divideFloats(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    float x = atof(token);
    token = strtok(NULL, " ");
    float y = atof(token);
    return x / y;
}

void sleeps(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    int x = atoi(token);
    sleep(x);
}

uint64_t factHelper(int cur, uint64_t acc) {
    if (cur <= 1) {
        return acc;
    } else {
        return factHelper(cur - 1, acc * cur);
    }
}

uint64_t factorial(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    int x = atoi(token);
    return factHelper(x, 1);
}


int isCommandValid(char *cmd) {
    char *commands[] = {"add", "multiply", "divide", "sleep", "factorial", "exit\n", "quit\n"};
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
    char msgCopy[BUFSIZE];
    char answer[BUFSIZE];
    if (create_server("127.0.0.1", atoi(argv[1]), &sockFd) < 0) {
        perror("listen socket create error\n");
        return EXIT_FAILURE;
    }
    
    while (1) { //todo check if we want to put it here
        int socket = accept_connection(sockFd, &clientFd);
        if (socket < 0) {
            perror("listen socket bind error\n");
            return EXIT_FAILURE;
        }
        int pid = fork();
        if(pid ==0){
            close(sockFd);
            while (1){
                memset(msg, 0, sizeof(msg));
                ssize_t byteCount = recv_message(clientFd, msg, BUFSIZE);
    
                dup2(clientFd, fileno(stdout));
                strcpy(msgCopy, msg);
                char *token = strtok(msg, " ");
                int result = isCommandValid(token);
                if (result == 1) {
                    if (isInputLengthValid(msg, 2)) {
                        sprintf(answer, "%d", addInts(msgCopy));
                    } else {
                        sprintf(answer, "%s", "not valid ADD");
                    }
                } else if (result == 2) {
                    if (isInputLengthValid(msg, 2)) {
                        sprintf(answer, "%d", multiplyInts(msgCopy));
                    } else {
                        sprintf(answer, "%s", "not valid multiply");
                    }
                } else if (result == 3) {
                    if (isInputLengthValid(msg, 2)) {
                        strcpy(msg, msgCopy);
                        if (isValidDivition(msg)) {
                            sprintf(answer, "%f", divideFloats(msgCopy));
                        } else {
                            sprintf(answer, "%s", "Cannot divide by 0!");
                        }
                    } else {
                        sprintf(answer, "%s", "not valid divide");
                    }
                } else if (result == 4) {
                    if (isInputLengthValid(msg, 1)) {
                        sleeps(msgCopy);
                        sprintf(answer, "%s", " ");
                    } else {
                        sprintf(answer, "%s", "not valid sleep");
                    }
                } else if (result == 5) {
                    if (isInputLengthValid(msg, 1)) {
                        sprintf(answer, "%lu", factorial(msgCopy));
                    } else {
                        sprintf(answer, "%s", "not valid factorial");
                    }
                } else if (result == 6) {
                    sprintf(answer, "%s", "exiting...");
                    return 0;
                } else if(result == 7){
                    sprintf(answer, "quitting");
                    exit(0);
                }else {
                    sprintf(answer, "%s", "not valid command");
                }
    
                if (byteCount <= 0) {
                    break;
                }
                send_message(clientFd, answer, strlen(answer));
                // indicate end of command output
                fflush(stdout);
            }
        } else{
            close(socket);
        }
       
    }
    return EXIT_SUCCESS;
}