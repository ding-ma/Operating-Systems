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
#include <signal.h>
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
    int cpuTaskId;
    char *taskStack;
    void *taskFunction;
    ucontext_t context;
};

struct ioTask {
    int ioTaskId;
    int taskType; //1: read, 2:write, 3:connect
    char *firstArg;
    int secondArg;
    struct queue_entry *cpuEntry;
};
//  todo wait for shutdown signal

/*
 * if the context of one is finished, we need to remove it from the qeueue. not put it at the end.
 */


void *cpuExecutor(void *args) {
    printf("CPU Thread start\n");
    while (1) {
        usleep(100);
        struct queue_entry *next = queue_peek_front(&cpuQueue);
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
    bool isConnectionSuccess = 0;
    srand(time(NULL)); //used to simulate read()
    
    //todo need a lock
    while (1) {
        usleep(200);
        struct queue_entry *next = queue_peek_front(&ioQueue);
        if (next != NULL) {
            struct ioTask *ioToExecute = (struct ioTask *) (next->data);
            if (ioToExecute->taskType == 1 && isConnectionSuccess) { //read
                //simulates a slow blocking read between 1 and 10s
//                int simulateRead = rand() % 10 + 1;
//                printf("reading for %d\n", simulateRead);
//                sleep(simulateRead);
//                strcpy(ioToExecute->firstArg, "AAAAAAAAA");
//                ioToExecute->secondArg = sizeof("AAA msg received %s\n");
    
                memset(readBuffer, 0, sizeof(READSIZE));
                while (1) {
                    ssize_t byte_count = recv_message(socketFd, readBuffer, sizeof(READSIZE));
                    if (byte_count > 0) {
                        printf("READ %s\n", readBuffer);
                        strcpy(ioToExecute->firstArg, readBuffer);
                        break;
                    }
                }
    
                queue_insert_tail(&cpuQueue, ioToExecute->cpuEntry);
                queue_pop_head(&ioQueue);
            }
            if (ioToExecute->taskType == 2 && isConnectionSuccess) { //write
                send_message(socketFd, ioToExecute->firstArg, ioToExecute->secondArg);
                printf("Message Sent to server...\n");
                queue_pop_head(&ioQueue);
            }
            
            if (ioToExecute->taskType == 3) {
                if (connect_to_server(ioToExecute->firstArg, ioToExecute->secondArg, &socketFd) < 0) {
                    printf("Could not connect to server %s:%d !\n", ioToExecute->firstArg, ioToExecute->secondArg);
                    printf("Skipping the current IO tasks \n");
                } else {
                    printf("Connected To Server!\n");
                    isConnectionSuccess = 1;
                }
                queue_pop_head(&ioQueue);
            }
            if (!isConnectionSuccess) {
                queue_pop_head(&ioQueue);
            }
        }
    }
    pthread_exit(NULL);
}

void sut_init() {
    cpuTaskCounter = 0;
    ioTaskCounter = 0;
    
    cpuQueue = queue_create();
    queue_init(&cpuQueue);
    
    pthread_mutex_lock(&cpuLock);
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
    
    queue_insert_tail(&cpuQueue, queue_new_node(t));
    printf("Task Created\n");
    
    //check for error. if there is, return false.
    return true;
}

void sut_yield() {
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    queue_insert_tail(&cpuQueue, head);
    
    struct cpuTask *toSwap = (struct cpuTask *) (head->data);
    swapcontext(&toSwap->context, &main);
}

//only difference is that when a cpuTask calls exit, we dont add it to the queue
void sut_exit() {
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    struct cpuTask *toSwap = (struct cpuTask *) (head->data);
    //frees memory as we dont need those pointers anymore - the task is finished
    free(head);
    free(toSwap);
    swapcontext(&toSwap->context, &main);
}

void sut_shutdown() {
    //wait until there are no more elements in queue.
    //todo implement lock
    while (queue_peek_front(&cpuQueue) != NULL || queue_peek_front(&ioQueue) != NULL) {
        usleep(5000);
    }
    pthread_cancel(cpuThread);
    pthread_cancel(ioThread);
    pthread_join(cpuThread, NULL);
    pthread_join(ioThread, NULL);
    printf("CPU and IO thread Terminated\n");
    
    pthread_mutex_destroy(&cpuLock);
    pthread_mutex_destroy(&ioLock);
}

void sut_open(char *dest, int port) {
    struct ioTask *ioTask;
    ioTask = malloc(sizeof(struct ioTask));
    
    ioTask->ioTaskId = ioTaskCounter++;
    ioTask->taskType = 3;
    ioTask->firstArg = dest;
    ioTask->secondArg = port;
    ioTask->cpuEntry = queue_peek_front(&cpuQueue);
    
    queue_insert_tail(&ioQueue, queue_new_node(ioTask));
}

void sut_write(char *buf, int size) {
    struct ioTask *ioTask;
    ioTask = malloc(sizeof(struct ioTask));
    
    ioTask->ioTaskId = ioTaskCounter++;
    ioTask->taskType = 2;
    ioTask->firstArg = buf;
    ioTask->secondArg = size;
    ioTask->cpuEntry = queue_peek_front(&cpuQueue);
    
    queue_insert_tail(&ioQueue, queue_new_node(ioTask));
}

void sut_close() {
    close(socketFd); //this will close connection between server and SUT. it will also terminate the server.
    printf("Connection closed\n");
}

char *sut_read() {
    //perform read from server
    
    struct ioTask *ioTask;
    ioTask = malloc(sizeof(struct ioTask));
    
    struct queue_entry *cpu = queue_pop_head(&cpuQueue);
    ioTask->ioTaskId = ioTaskCounter++;
    ioTask->taskType = 1;
    ioTask->firstArg = (char *) malloc(READSIZE);
    ioTask->secondArg = sizeof(ioTask->firstArg);
    ioTask->cpuEntry = cpu;
    
    queue_insert_tail(&ioQueue, queue_new_node(ioTask));
    struct cpuTask *t = (struct cpuTask *) cpu->data;
    
    swapcontext(&t->context, &main);
    
    return ioTask->firstArg;
}