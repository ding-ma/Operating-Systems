#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


#include "queue.h"
#include "sut.h"


struct queue TO_C_Q, TO_IO_Q; //declaring queue as a global var


pthread_t C_EXEC, I_EXEC;
ucontext_t C_Context, I_Context;
TCB *next;
TCB *curr;
int numtask, curtask;
int sockfd; //only need to make one global socket since we are dealing with one at once
bool shut = false;
bool open = false;

pthread_mutex_t c_lock, i_lock;

IOB *io_task;


//initialization
void sut_init() {
    numtask = 0;
    TO_C_Q = queue_create(); //create queue
    queue_init(&TO_C_Q);
    
    TO_IO_Q = queue_create(); //create queue
    queue_init(&TO_IO_Q);
    
    // printf("BEFORE INI THREADS\n");
    int cthread = pthread_create(&C_EXEC, NULL, c_exec, NULL);
    int ithread = pthread_create(&I_EXEC, NULL, i_exec, NULL);
    
    pthread_mutex_init(&c_lock, NULL);
    pthread_mutex_init(&i_lock, NULL);
    
    
    // if(cthread !=0 || ithread != 0){
    //     fprintf(stderr, "Fatal error in thread creation. Program exited\n");
    //     exit(1);
    // }
    
}


bool sut_create(sut_task_f fn) {
    
    TCB *task_ptr = malloc(sizeof(TCB));
    
    getcontext(&(task_ptr->threadcontext));
    
    task_ptr->threadid = numtask;
    task_ptr->threadstack = (char *) malloc(1024 * 64);
    
    task_ptr->threadcontext.uc_stack.ss_sp = task_ptr->threadstack;
    task_ptr->threadcontext.uc_stack.ss_size = 1024;
    task_ptr->threadcontext.uc_stack.ss_flags = 0;
    
    task_ptr->threadfunc = fn;
    
    makecontext(&(task_ptr->threadcontext), fn, 0);
    
    pthread_mutex_lock(&c_lock);
    queue_insert_tail(&TO_C_Q, queue_new_node(task_ptr)); //insert the initialized task into the task queue
    pthread_mutex_unlock(&c_lock);
    
    numtask++;
    
    return true;
}


void sut_yield() {
    
    struct queue_entry *node = queue_new_node(next);
    
    pthread_mutex_lock(&c_lock);
    queue_insert_tail(&TO_C_Q, node);
    pthread_mutex_unlock(&c_lock);
    
    swapcontext(&(next->threadcontext), &C_Context);
    
}

void sut_exit() {
    
    swapcontext(&(next->threadcontext), &C_Context);
    
}

/*takes job from queue in a while loop, swap the context to the job
if the queue is empty, goes to sleep for 100ms. When shutdown is called
by job, breaks out of the loop.
*/

void *c_exec() {
    
    while (true) {
        if (!STAILQ_EMPTY(&TO_C_Q)) {
            
            pthread_mutex_lock(&c_lock);
            struct queue_entry *ptr = queue_pop_head(&TO_C_Q);
            pthread_mutex_unlock(&c_lock);
            next = ptr->data;
            swapcontext(&C_Context, &(next->threadcontext));
        } else {
            if (shut) {
                break;
            }
            usleep(100);
        }
    }
    
}

void *i_exec() {
    
    while (true) {
        
        struct queue_entry *front = queue_peek_front(&TO_IO_Q);
        
        
        if (front != NULL) {
            
            //perform reading
            if (io_task->instruction == 1) {
                
                // recv(sockfd, cur_task->buf , sizeof(cur_task->buf), 0);
                
                // pthread_mutex_lock(&i_lock);
                // queue_insert_tail(&TO_C_Q, queue_new_node(cur_task->c_task));
                // pthread_mutex_unlock(&i_lock);
                
            }
            //perform writing
            if (io_task->instruction == 2) {
                
                pthread_mutex_lock(&i_lock);
                struct queue_entry *ptr = queue_pop_head(&TO_IO_Q);
                pthread_mutex_unlock(&i_lock);
                char *curr_msg = ptr->data;
                
                send(sockfd, curr_msg, strlen(curr_msg), 0);
                
            }
            
            struct queue_entry *node = queue_new_node(io_task->c_task);
            pthread_mutex_lock(&c_lock);
            queue_insert_tail(&TO_C_Q, node);
            pthread_mutex_unlock(&c_lock);
        }
    }
    
}


//code recycled from assignment 1
int connect_to_server(const char *host, uint16_t port, int *sockfd) {
    struct sockaddr_in server_address = {0};
    
    // create a new socket
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        perror("Failed to create a new socket\n");
        return -1;
    }
    // connect to server
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, host, &(server_address.sin_addr.s_addr));
    server_address.sin_port = htons(port);
    if (connect(*sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Failed to connect to server\n");
        return -1;
    }
    return 0;
}


void sut_open(char *dest, int port) {
    
    
    // printf("OPENED\n");
    IOB *io_task = (IOB *) malloc(sizeof(IOB));
    
    io_task->dest = dest;
    io_task->port = port;
    
    connect_to_server(dest, port, &sockfd);
    open = true;
    
    // pthread_mutex_lock(&i_lock);
    
    // // TCB *old_task = (TCB *)malloc(sizeof(TCB));
    // // old_task = (TCB *)(queue_pop_head(&TO_C_Q))->data;
    // // pthread_mutex_unlock(&i_lock);
    
    // // io_task->c_task = malloc(sizeof(TCB));
    // io_task->c_task = next;
    
    // pthread_mutex_lock(&i_lock);
    // queue_insert_tail(&TO_IO_Q, queue_new_node(io_task));
    // pthread_mutex_unlock(&i_lock);
    // swapcontext(&(io_task->c_task->threadcontext), &C_Context);
    
}


char *sut_read() {

//     if(!open){
//         printf("Need to connect to server first. \n");
//     }
//     else{
//         pthread_mutex_lock(&i_lock);
//         IOB *io_task = (IOB *)queue_pop_head(&TO_IO_Q)->data;
//         pthread_mutex_unlock(&i_lock);

//         io_task->instruction = 1;
//         io_task->size = 1024*64;
//         io_task->buf = malloc(io_task->size);

//         pthread_mutex_lock(&i_lock);        
//         queue_insert_head(&TO_IO_Q, (struct queue_entry *)io_task);
//         pthread_mutex_unlock(&i_lock);
//         swapcontext(&(io_task->c_task->threadcontext), &I_Context);
//     }
    
}


void sut_write(char *buf, int size) {
    
    printf("ENTERED WRITE\n");
    // pthread_mutex_lock(&i_lock);
    // IOB *io_task = malloc(sizeof(IOB));
    // (IOB *)queue_pop_head(&TO_IO_Q)->data;
    // pthread_mutex_unlock(&i_lock);
    
    
    if (!open) {
        printf("Need to connect to server first. \n");
    } else {
        io_task = malloc(sizeof(IOB));
        io_task->instruction = 2;
        io_task->size = size;
        io_task->buf = (char *) malloc(size);
        strcpy(io_task->buf, buf);
        
        // printf("%s", io_task->buf);
        struct queue_entry *node = queue_new_node(io_task->buf);
        pthread_mutex_lock(&i_lock);
        queue_insert_head(&TO_IO_Q, node);
        pthread_mutex_unlock(&i_lock);
        
        swapcontext(&(io_task->c_task->threadcontext), &C_Context);
    }
}


void sut_close() {
    
    pthread_mutex_lock(&i_lock);
    IOB *io_task = (IOB *) queue_pop_head(&TO_IO_Q)->data;
    pthread_mutex_unlock(&i_lock);
    
    if (!open) {
        printf("Need to connect to server first. \n");
    } else {
        close(sockfd);
    }
}


void sut_shutdown() {
    shut = true;
    
    pthread_join(C_EXEC, NULL);
    
}



// void printout_q()
// {

//   if (!STAILQ_EMPTY(&TO_C_Q))
//   {
//     struct queue_entry *cur_node = queue_peek_front(&TO_C_Q);
//     TCB *cur = (cur_node)->data;
//     printf("queue head: %d \n", cur->threadid);

//     struct queue_entry *en = STAILQ_NEXT(cur_node, entries);

//     int i = 2;
//     while (en != NULL)
//     {
//       cur = en->data;
//       printf("queue %dth element: %d \n", i, cur->threadid);

//       en = STAILQ_NEXT(en, entries);

//       i++;
//     }

//     printf("\n");
//   }
//   else
//   {
//     printf("empty queue\n");
//   }
// }