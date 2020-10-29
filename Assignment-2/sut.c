#include "sut.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include <stdbool.h>
#include <ucontext.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "socket_lib.h"

struct queue cpuQueue;
struct queue ioQueue;
static ucontext_t main;
static pthread_t cpuThread, ioThread;
int cpuTaskCounter, ioTaskCounter;
pthread_mutex_t cpuLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ioLock = PTHREAD_MUTEX_INITIALIZER;
int socketFd;

struct cpuTask {
    int cpuTaskId; //used for debug
    char *taskStack;
    void *taskFunction;
    ucontext_t context;
};

struct ioTask {
    int ioTaskId; //used for debug
    int taskType; //1: read, 2:write, 3:connect
    char *firstArg;
    int secondArg;
    struct queue_entry *cpuEntry;
};

void *cpuExecutor(void *args) {
    printf("CPU Thread start\n");
    while (1) {
        usleep(100);
        pthread_mutex_lock(&cpuLock);
        struct queue_entry *next = queue_peek_front(&cpuQueue);
        pthread_mutex_unlock(&cpuLock);
    
        if (next != NULL) {
            struct cpuTask *toExecute = (struct cpuTask *) (next->data);
            swapcontext(&main, &toExecute->context);
        }
    }
    pthread_exit(NULL);
}

void *ioExecutor(void *args) {
    printf("IO Thread start \n");
    char readBuffer[READSIZE];
    bool isConnectionSuccess = 0; //if the connection to server was no successful, we simply pop all the elements of the IO queue
    
    while (1) {
        usleep(200);
        pthread_mutex_lock(&ioLock);
        struct queue_entry *next = queue_peek_front(&ioQueue);
        pthread_mutex_unlock(&ioLock);
        
        if (next != NULL) {
            struct ioTask *ioToExecute = (struct ioTask *) (next->data);
            
            if (isConnectionSuccess) {
                if (ioToExecute->taskType == 1) { //read
                    memset(readBuffer, 0, sizeof(READSIZE));
                    while (1) {
                        ssize_t byte_count = recv_message(socketFd, readBuffer, READSIZE);
                        if (byte_count > 0) {
                            strcpy(ioToExecute->firstArg, readBuffer);
                            break;
                        }
                    }
                    pthread_mutex_lock(&cpuLock);
                    queue_insert_tail(&cpuQueue, ioToExecute->cpuEntry);
                    pthread_mutex_unlock(&cpuLock);
                }
                
                if (ioToExecute->taskType == 2) { //write
                    send_message(socketFd, ioToExecute->firstArg, ioToExecute->secondArg);
                    printf("Message Sent to server...\n");
                }
            }
            
            if (ioToExecute->taskType == 3) { //connect
                if (connect_to_server(ioToExecute->firstArg, ioToExecute->secondArg, &socketFd) < 0) {
                    printf("Could not connect to server %s:%d !\n", ioToExecute->firstArg, ioToExecute->secondArg);
                    printf("Skipping the current IO tasks \n");
                } else {
                    printf("Connected To Server!\n");
                    isConnectionSuccess = 1;
                }
            }
            
            //remove item from the queue after being processed
            pthread_mutex_lock(&ioLock);
            queue_pop_head(&ioQueue);
            pthread_mutex_unlock(&ioLock);
            
        }
    }
    pthread_exit(NULL);
}

void sut_init() {
    cpuTaskCounter = 0;
    ioTaskCounter = 0;
    
    cpuQueue = queue_create();
    queue_init(&cpuQueue);
    
    if (pthread_create(&cpuThread, NULL, cpuExecutor, NULL) != 0) {
        perror("Could not create CPU thread\n");
        exit(1);
    }
    
    ioQueue = queue_create();
    queue_init(&ioQueue);
    
    if (pthread_create(&ioThread, NULL, ioExecutor, NULL) != 0) {
        perror("Could not create IO thread");
        exit(1);
    }
    printf("Library initialized\n");
}

bool sut_create(sut_task_f fn) {
    struct cpuTask *t;
    t = malloc(sizeof(struct cpuTask));
    
    //creates the tasks as well as its context
    getcontext(&t->context);
    t->cpuTaskId = cpuTaskCounter;
    t->taskStack = (char *) malloc(STACK_SIZE);
    t->context.uc_stack.ss_sp = t->taskStack;
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = 0;
    t->context.uc_stack.ss_flags = 0;
    t->taskFunction = fn;
    makecontext(&(t->context), fn, 1, t);
    cpuTaskCounter++;
    
    pthread_mutex_lock(&cpuLock);
    queue_insert_tail(&cpuQueue, queue_new_node(t));
    pthread_mutex_unlock(&cpuLock);
    
    printf("Task Created\n");
    
    //check for error. if there is, return false.
    return true;
}

void sut_yield() {
    pthread_mutex_lock(&cpuLock);
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    queue_insert_tail(&cpuQueue, head);
    pthread_mutex_unlock(&cpuLock);
    
    struct cpuTask *toSwap = (struct cpuTask *) (head->data);
    swapcontext(&toSwap->context, &main);
}

//only difference is that when a cpuTask calls exit, we dont add it back to the queue
void sut_exit() {
    pthread_mutex_lock(&cpuLock);
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    pthread_mutex_unlock(&cpuLock);
    
    struct cpuTask *toSwap = (struct cpuTask *) (head->data);
    
    swapcontext(&toSwap->context, &main);
    
    //free memory as we dont need those pointers anymore - the task is finished
    free(head);
    free(toSwap);
}

void sut_shutdown() {
    
    //check the queues constantly to see if both are empty
    while (queue_peek_front(&cpuQueue) != NULL || queue_peek_front(&ioQueue) != NULL) {
        usleep(5000);
    }
    
    //if both are empty, we can now process to shutdown
    pthread_cancel(cpuThread);
    pthread_cancel(ioThread);
    pthread_join(cpuThread, NULL);
    pthread_join(ioThread, NULL);
    printf("CPU and IO thread Terminated\n");
    
    //destroys the lock as clean up process
    pthread_mutex_destroy(&cpuLock);
    pthread_mutex_destroy(&ioLock);
}

void sut_open(char *dest, int port) {
    struct ioTask *ioTask;
    ioTask = malloc(sizeof(struct ioTask));
    
    //creates a task to go connect on server ofr IO.
    // if the IO blocks or the server fails to connect, it will not affect the cpu thread
    ioTask->ioTaskId = ioTaskCounter++;
    ioTask->taskType = 3;
    ioTask->firstArg = dest;
    ioTask->secondArg = port;
    
    pthread_mutex_lock(&cpuLock);
    ioTask->cpuEntry = queue_peek_front(&cpuQueue);
    pthread_mutex_unlock(&cpuLock);
    
    pthread_mutex_lock(&ioLock);
    queue_insert_tail(&ioQueue, queue_new_node(ioTask));
    pthread_mutex_unlock(&ioLock);
    
}

void sut_write(char *buf, int size) {
    struct ioTask *ioTask;
    ioTask = malloc(sizeof(struct ioTask));
    
    ioTask->ioTaskId = ioTaskCounter++;
    ioTask->taskType = 2;
    ioTask->firstArg = buf;
    ioTask->secondArg = size;
    
    //store the cpu entry ptr into the IO
    pthread_mutex_lock(&cpuLock);
    ioTask->cpuEntry = queue_peek_front(&cpuQueue);
    pthread_mutex_unlock(&cpuLock);
    
    pthread_mutex_lock(&ioLock);
    queue_insert_tail(&ioQueue, queue_new_node(ioTask));
    pthread_mutex_unlock(&ioLock);
}


void sut_close() {
    close(socketFd); //this will close connection between server and SUT. it will also terminate the server.
    printf("Connection closed\n");
}

char *sut_read() {
    //perform read from server
    
    struct ioTask *ioTask;
    ioTask = malloc(sizeof(struct ioTask));
    
    pthread_mutex_lock(&cpuLock);
    struct queue_entry *cpu = queue_pop_head(&cpuQueue);
    pthread_mutex_unlock(&cpuLock);
    
    ioTask->ioTaskId = ioTaskCounter++;
    ioTask->taskType = 1;
    ioTask->firstArg = (char *) malloc(READSIZE);
    ioTask->secondArg = sizeof(ioTask->firstArg);
    ioTask->cpuEntry = cpu;
    
    pthread_mutex_lock(&ioLock);
    queue_insert_tail(&ioQueue, queue_new_node(ioTask));
    pthread_mutex_unlock(&ioLock);
    
    struct cpuTask *t = (struct cpuTask *) cpu->data;
    swapcontext(&t->context, &main);
    
    //return value of the read is stored in the first argument
    return ioTask->firstArg;
}