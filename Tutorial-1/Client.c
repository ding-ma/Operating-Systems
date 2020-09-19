#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

const char kEndOfTransfer = 26;

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
    ssize_t inputLineLengthRead = -1;
    size_t inputLengthAllocated = 0;
    // prompt user's input command until EOF
    while ((inputLineLengthRead = getline(&inputLine, &inputLengthAllocated, stdin)) != -1) {
        // send command to server
        send(fdSocketClient, inputLine, inputLineLengthRead, 0);
        // print/forward everything sent from server to console
        char recvChar;
        while (recv(fdSocketClient, &recvChar, 1, 0) > 0) {
            if (recvChar == kEndOfTransfer) break;
            putc(recvChar, stdout);
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