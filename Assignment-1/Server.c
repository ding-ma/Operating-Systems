#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "a1_lib.h"

#define BUFSIZE   2024


int addInts(char *cmd) {
    char *token = strtok(cmd, " ");
    int x = atoi(token);
    token = strtok(NULL, " ");
    int y = atoi(token);
    
    return x + y;
}

int multiplyInts(char *cmd) {
    char *token = strtok(cmd, " ");
    int x = atoi(token);
    token = strtok(NULL, " ");
    int y = atoi(token);
    
    return x * y;
}

int isValidDivition(char *cmd) {
    fprintf(stderr, "in valid div %s\n", cmd);
    char *token = strtok(cmd, " ");
    float x = atof(token);
    token = strtok(NULL, " ");
    float y = atof(token);
    
    return y != 0.0;
}

float divideFloats(char *cmd) {
    char *token = strtok(cmd, " ");
    float x = atof(token);
    token = strtok(NULL, " ");
    float y = atof(token);
    
    return x / y;
}

void sleeps(char *cmd) {
    int x = atoi(cmd);
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
    int x = atoi(cmd);
    return factHelper(x, 1);
}


int isCommandValid(char *cmd) {
    char *commands[] = {"add", "multiply", "divide", "sleep", "factorial", "exit\n", "shutdown\n"};
    int len = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < len; ++i) {
        if (strcmp(commands[i], cmd) == 0) {
            return i + 1;
        }
    }
    return 0;
}

int isInputLengthValid(char *cmd, int expectedLength) {
    fprintf(stderr, "command in %s", cmd);
    char *token = strtok(cmd, " ");
    int curr = 0;
    while (token != NULL) {
        token = strtok(NULL, " ");
        curr++;
    }
    fprintf(stderr, "numb command %d", curr);
    return curr == expectedLength;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    
    int sockFd, clientFd;
    char msg[BUFSIZE];
    char msgCopy[BUFSIZE];
    char answer[BUFSIZE];
    message *sentMsg;
    if (create_server(argv[1], atoi(argv[2]), &sockFd) < 0) {
        perror("listen socket create error\n");
        return EXIT_FAILURE;
    }
//    freopen("/dev/null", "w", stderr); //surpress debug mode
    fprintf(stderr, "Server listening on %s:%s \n", argv[1], argv[2]);
    
    while (1) {
        int socket = accept_connection(sockFd, &clientFd);
        if (socket < 0) {
            perror("listen socket bind error\n");
            return EXIT_FAILURE;
        }
        int pid = fork();
        if (pid == 0) {
            close(sockFd);
            while (1) {
                memset(msg, 0, sizeof(msg));
                ssize_t byteCount = recv_message(clientFd, msg, BUFSIZE);
                sentMsg = (message *) msg;
    
                dup2(clientFd, fileno(stdout));
    
                fprintf(stderr, "msg received: func %s ,args %s \n", sentMsg->function, sentMsg->arguments);
    
                strcpy(msgCopy, sentMsg->arguments);
                int result = isCommandValid(sentMsg->function);
                fprintf(stderr, "s:%s   d:%d \n", sentMsg->arguments, result);
    
                if (result == 1) {
                    if (isInputLengthValid(sentMsg->arguments, 3)) {
                        sprintf(answer, "%d", addInts(msgCopy));
                    } else {
                        sprintf(answer, "add needs 2 numbers");
                    }
                } else if (result == 2) {
                    if (isInputLengthValid(sentMsg->arguments, 3)) {
                        sprintf(answer, "%d", multiplyInts(msgCopy));
                    } else {
                        sprintf(answer, "multiple needs 2 numbers");
                    }
                } else if (result == 3) {
                    if (isInputLengthValid(sentMsg->arguments, 3)) {
                        strcpy(msg, msgCopy);
                        if (isValidDivition(msgCopy)) {
                            sprintf(answer, "%f", divideFloats(msg));
                        } else {
                            sprintf(answer, "Error: Division by zero");
                        }
                    } else {
                        sprintf(answer, "divide needs 2 numbers");
                    }
                } else if (result == 4) {
                    if (isInputLengthValid(sentMsg->arguments, 2)) {
                        sleeps(msgCopy);
                        sprintf(answer, " ");
                    } else {
                        sprintf(answer, "sleep needs 1 number");
                    }
                } else if (result == 5) {
                    if (isInputLengthValid(sentMsg->arguments, 2)) {
                        sprintf(answer, "%lu", factorial(msgCopy));
                    } else {
                        sprintf(answer, "factorial needs 1 number");
                    }
                } else if (result == 6) {
                    sprintf(answer, " ");
                    close(socket);
                    return EXIT_SUCCESS;
                } else if (result == 7) {
                    sprintf(answer, " ");
                    fprintf(stderr, "here %d %d\n", getpid(), getppid());
                    kill(0, SIGTERM); //we want to kill all the current processes so we use 0
                } else {
                    sprintf(answer, "Error: Command \"%s\" not found", sentMsg->function);
                }
                
                if (byteCount <= 0) {
                    break;
                }
                send_message(clientFd, answer, strlen(answer));
                // indicate end of command output
                fflush(stdout);
            }
        } else {
            //close the socket for the parent as we dont need it
            close(socket);
        }
    }
    
}