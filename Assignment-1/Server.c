#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "a1_lib.h"

#define BUFSIZE   1024


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
    cmd = strtok(NULL, " ");
    int curr = 0;
    while (cmd != NULL) {
        cmd = strtok(NULL, " ");
        curr++;
    }
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
    message  *sentMsg;
    if (create_server(argv[1], atoi(argv[2]), &sockFd) < 0) {
        perror("listen socket create error\n");
        return EXIT_FAILURE;
    }
//    freopen("/dev/null", "w", stderr); //surpress debug mode
    fprintf(stderr, "Server listening on %s:%s", argv[1], argv[2]);
    
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
                sentMsg = (message *)msg;
                
                dup2(clientFd, fileno(stdout));
                strcpy(msgCopy, sentMsg->input);
                char *token = strtok(sentMsg->input, " ");
                int result = isCommandValid(token);
                fprintf(stderr, "s:%s   d:%d \n", sentMsg->input, result);
                
                if (result == 1) {
                    if (isInputLengthValid(sentMsg->input, 2)) {
                        sprintf(answer, "%d", addInts(msgCopy));
                    } else {
                        sprintf(answer, "COMMAND NOT FOUND");
                    }
                } else if (result == 2) {
                    if (isInputLengthValid(sentMsg->input, 2)) {
                        sprintf(answer, "%d", multiplyInts(msgCopy));
                    } else {
                        sprintf(answer, "COMMAND NOT FOUND");
                    }
                } else if (result == 3) {
                    if (isInputLengthValid(sentMsg->input, 2)) {
                        strcpy(msg, msgCopy);
                        if (isValidDivition(sentMsg->input)) {
                            sprintf(answer, "%f", divideFloats(msgCopy));
                        } else {
                            sprintf(answer, "Error: Division by zero");
                        }
                    } else {
                        sprintf(answer, "COMMAND NOT FOUND");
                    }
                } else if (result == 4) {
                    if (isInputLengthValid(sentMsg->input, 1)) {
                        sleeps(msgCopy);
                        sprintf(answer, " ");
                    } else {
                        sprintf(answer, "COMMAND NOT FOUND");
                    }
                } else if (result == 5) {
                    if (isInputLengthValid(sentMsg->input, 1)) {
                        sprintf(answer, "%lu", factorial(msgCopy));
                    } else {
                        sprintf(answer, "COMMAND NOT FOUND");
                    }
                } else if (result == 6) {
                    sprintf(answer, " ");
                    close(socket);
                    return EXIT_SUCCESS;
                } else if (result == 7) {
                    sprintf(answer, " ");
                    fprintf(stderr, "here %d %d\n", getpid(), getppid());
                    kill(getppid(), SIGTERM);
                } else {
                    sprintf(answer, "Error: Command \"%s\" not found", token);
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