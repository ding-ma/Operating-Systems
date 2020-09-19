#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int main(int argc, char *argv[]) {
    if (argc !=2){
        return EXIT_FAILURE;
    }
    int serverPort = atoi(argv[1]), fdListenSocket;
    if ((fdListenSocket = socket(PF_INET, SOCK_STREAM,0))<0) {
        perror("listen socket created error \n");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in serverSocket;
    memset(&serverSocket, 0, sizeof(struct sockaddr_in));
    serverSocket.sin_family = AF_INET;
    serverSocket.sin_port = htons(serverPort);
    
    //host to online long converts ip address to big endian
    serverSocket.sin_addr.s_addr = htonl(INADDR_ANY);
    //casting is fine as sockaddr is a sub-struct of sockaddr_in
    if (bind(fdListenSocket, (struct sockaddr *)(&serverSocket), sizeof(struct sockaddr)) <0){
        perror("listen socket bind error \n");
        return EXIT_FAILURE;
    }
    if (listen(fdListenSocket,16)<0){
        perror("listen socket listen error \n");
        return EXIT_FAILURE;
    }
    while (1){
        struct sockaddr_in clientConnectionAddress;
        socklen_t clientConnectionAddrLen;
        int fdClientConnectionSocket;
        if ((fdClientConnectionSocket = accept(fdListenSocket, (struct sockaddr *)
                (&clientConnectionAddress), &clientConnectionAddrLen)) < 0){
            perror("listen socket accept error \n");
            return EXIT_FAILURE;
        }
        //remember that everything is a file in linux
        FILE *fpClientSocket = fdopen(fdClientConnectionSocket, "r");
        char *inputLine = NULL;
        ssize_t inputLineLengthRead = -1;
        size_t inputLengthAllocated = 0;
        while ((inputLineLengthRead = getline(&inputLine, &inputLengthAllocated, fpClientSocket)) != -1){
//            write(fdClientConnectionSocket, inputLine, inputLineLengthRead +1);
            
            int childInfo = fork();
            if(childInfo < 0){
                return EXIT_FAILURE;
            } else if (childInfo ==0){
                dup2(fdClientConnectionSocket, fileno(stdout));
                system(inputLine);
                exit(EXIT_SUCCESS);
            } else {
            
            }
            
            free(inputLine);
            inputLine = NULL;
            inputLengthAllocated = 0;
        }
        fclose(fpClientSocket);
        close(fdClientConnectionSocket);
    }
    close(fdListenSocket);
    return EXIT_SUCCESS;
}
