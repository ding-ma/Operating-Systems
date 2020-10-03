#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "a1_lib.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE   1024

int main(void) {
    int sockfd, clientfd;
    char msg[BUFSIZE];
    const char *greeting = "hello, world\n";
    message *m;
    
    if (create_server("127.0.0.1", 6535, &sockfd) < 0) {
        fprintf(stderr, "oh no server\n");
        return -1;
    }
    
    while (1) {
        int socket = accept_connection(sockfd, &clientfd);
        if (socket < 0) {
            fprintf(stderr, "oh no connection\n");
            return -1;
        }
        int pid = fork();
        int rval;
        if (pid == 0) {
            close(sockfd);
            while (1) {
                memset(msg, 0, sizeof(msg));
                ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);
                
                m = (message *) msg;
                printf("%s", m->input);
                if (byte_count <= 0) {
                    break;
                }
                if (strcmp(msg, "e\n") == 0) {
                    close(socket);
                    return 10;
                }
                if (strcmp(msg, "q\n") == 0) {
                    printf("quitting");
                    exit(EXIT_SUCCESS);
                }
                printf("Client: %s\n", msg);
                send_message(clientfd, greeting, strlen(greeting));
            }
        } else {
            close(socket);
            int status;
    
//            while (1){
//                waitpid(pid, &status, WNOHANG);
//                if ( WIFEXITED(status) ){
//                    int exit_status = WEXITSTATUS(status);
//                    printf("Exit status of the child was %d\n",
//                           exit_status);
//                    break;
//                }
//            }
        }
    
       
    }
    
    return 0;
}

