#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

const char end = 26;

int main(int argc, char *argv[]){
    if (argc != 3){
        return EXIT_FAILURE;
    }
    int serverPort = atoi(argv[2]), fdSocketClient;
    if ((fdSocketClient = socket(PF_INET, SOCK_STREAM,0))<0){
        perror("client socket created error \n");
        return EXIT_FAILURE;
    }
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof( struct sockaddr_in));
    clientAddress.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &clientAddress.sin_addr);
    clientAddress.sin_port = htons(serverPort); //packing server ip
    if (connect(fdSocketClient, (struct sockaddr *)(&clientAddress), sizeof(struct sockaddr))){
        perror("client socket connect error \n");
        return EXIT_FAILURE;
    }
    
    char *inputLine = NULL;
    ssize_t inputLineLengthRead = -1;
    size_t inputLengthAllocated = 0;
    while ((inputLineLengthRead = getline(&inputLine, &inputLengthAllocated, stdin)) != -1) {
        send(fdSocketClient, inputLine, inputLineLengthRead,0);
        char receiveChar;
        while(recv(fdSocketClient, &receiveChar,1,0)>0){
            if(receiveChar == end) break;
            putc(receiveChar, stdout);
        }
        fflush(stdout);
        free(inputLine);
        inputLine = NULL;
        inputLengthAllocated = 0;
    }
    close(fdSocketClient);
    return EXIT_SUCCESS;
}