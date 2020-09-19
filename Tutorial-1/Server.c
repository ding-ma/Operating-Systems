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
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    int serverPort = atoi(argv[1]), fdListenSocket;
    // Socket: "pathway" for transmitting info between processes
    // 1. create (a null) socket for listening upcoming connections
    if ((fdListenSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("listen socket create error\n");
        return EXIT_FAILURE;
    }
    // initialize port and address
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    // listen on argv[1]
    serverAddress.sin_port = htons(serverPort);
    // listen for connections from all addresses (not a good practice in general)
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // 2. associate the previous socket to port argv[1] and IP addr of this machine
    if (bind(fdListenSocket, (struct sockaddr *) (&serverAddress), sizeof(struct sockaddr)) < 0) {
        perror("listen socket bind error\n");
        return EXIT_FAILURE;
    }
    // 3. ask the current process listen on this socket that each message receive triggers a kernel level event that will wake up this process
    if (listen(fdListenSocket, 16) < 0) {
        perror("listen socket listen error\n");
        return EXIT_FAILURE;
    }
    while (1) {
        struct sockaddr_in clientConnectionAddress;
        socklen_t clientConnectionAddrLen;
        int fdClientConnectionSocket;
        // 4. wait until there is a connection request posted, or not wait if there is actually one
        if ((fdClientConnectionSocket = accept(fdListenSocket, (struct sockaddr *) (&clientConnectionAddress),
                                               &clientConnectionAddrLen)) < 0) {
            perror("listen sock accpt error\n");
            return EXIT_FAILURE;
        }
        FILE *fpClientSocket = fdopen(fdClientConnectionSocket, "r");
        char *inputLine = NULL;
        ssize_t inputLineLengthRead = -1;
        size_t inputLengthAllocated = 0;
        // for each command read, until a end of file indicate that connection/socket closed
        while ((inputLineLengthRead = getline(&inputLine, &inputLengthAllocated, fpClientSocket)) != -1) {
            // init a child process to run the command
            // Q: you can do system within the current process, so why not?
            // A: i don't want to make my main process dirty
            int childInfo = fork();
            if (childInfo < 0) {
                return EXIT_FAILURE;
            } else if (childInfo == 0) {
                // child: 
                // replacing the stdout's fd with socket's fd such that the stdout of command will be forwarded to the client through the socket
                dup2(fdClientConnectionSocket, fileno(stdout));
                // execute command
                system(inputLine);
                // indicate end of command output
                putc(kEndOfTransfer, stdout);
                fflush(stdout);
                exit(EXIT_SUCCESS);
            } else {
                // wait until child finished
                wait(NULL);
            }
            // free up & reinitialize inputLine and inputLengthAllocated used by getline
            free(inputLine);
            inputLine = NULL;
            inputLengthAllocated = 0;
        }
        fclose(fpClientSocket);
    }
    close(fdListenSocket);
    return EXIT_SUCCESS;
}