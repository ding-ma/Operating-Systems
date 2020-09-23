#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "a1_lib.h"


const char kEndOfTransfer = 26;


void sendCommand(int fdSocketClient,char* inputLine, char inputLineLengthRead) {
    send(fdSocketClient, inputLine, inputLineLengthRead, 0);
    
    // print/forward everything sent from server to console
    char recvChar;
    while (recv(fdSocketClient, &recvChar, 1, 0) > 0) {
        if (recvChar == kEndOfTransfer){
            break;
        }
        putc(recvChar, stdout);
    }
}

//void sendCommandIfValid(char *cmd, int numbArgs, int fdSocketClient, char* inputLine, char inputLineLengthRead) {
//    int counter = 0;
//    cmd = strtok(NULL, " ");
//    while (cmd != NULL) {
//        printf("TOKEN------- %s\n", cmd); //printing each token
//        cmd = strtok(NULL, " ");
//        counter++;
//    }
//    if(counter == numbArgs){
//        sendCommand(fdSocketClient,inputLine, inputLineLengthRead);
//    } else{
//        printf("There is an error in your command \n");
//    }
//}


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
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    
    int serverPort = atoi(argv[2]), fdSocketClient;
    // init the socket
    if ((fdSocketClient = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("client socket create error\n");
        return EXIT_FAILURE;
    }
    // init connection info
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(struct sockaddr_in));
    clientAddress.sin_family = AF_INET;
    // set server's IP address
    inet_pton(AF_INET, argv[1], &clientAddress.sin_addr);
    // set server's port
    clientAddress.sin_port = htons(serverPort);
    // connect to server using information above
    if (connect(fdSocketClient, (struct sockaddr *) (&clientAddress), sizeof(struct sockaddr))) {
        perror("client socket connect error\n");
        return EXIT_FAILURE;
    }
    char *inputLine = NULL;
    ssize_t inputLineLengthRead = 0;
    size_t inputLengthAllocated = 0;
    // prompt user's input command until EOF
    while (inputLineLengthRead != -1) {
        // send command to server
        printf(">> ");
        inputLineLengthRead = getline(&inputLine, &inputLengthAllocated, stdin);
        
        char duplicate[300];
        strcpy(duplicate,inputLine);
        char *token = strtok(inputLine, " ");
        
        if (isCommandValid(token)) { //check if the command is a valid one
            sendCommand(fdSocketClient, duplicate, inputLineLengthRead);
        } else if (strcmp(token, "exit\n") == 0) {
            break;
        } else {
            printf("Wrong command \n");
        }
        
        fflush(stdout);
        // free up & reinitialize inputLine and inputLengthAllocated used by getline
        free(inputLine);
        inputLine = NULL;
        inputLengthAllocated = 0;
    }
    // close socket to avoid file descriptor exaustion problem (email Prof. Muthucumaru Maheswaran for more detail regarding this problem)
    close(fdSocketClient);
    return EXIT_SUCCESS;
}