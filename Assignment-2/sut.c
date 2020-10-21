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

struct queue cpuQueue;
struct queue ioQueue;
static ucontext_t main;
static pthread_t cpuThread, ioThread;
pthread_mutex_t exitCall;
pthread_mutex_t addedFirstElement;

struct job {
    ucontext_t context;
    char stack[16 * 1024];
};

// todo wait for item to be added to list
//  todo wait for shutdown signal

/*
 * if the context of one is finished, we need to remove it from the qeueue. not put it at the end.
 */


void *cpuTask(void *args) {
    pthread_mutex_lock(&addedFirstElement);
    while (1) {
        struct queue_entry *next = queue_peek_front(&cpuQueue);
        if (next != NULL) {
            struct job *toExecute = (struct job *) (next->data);
            swapcontext(&main, &toExecute->context);
        } else {
            if (pthread_cancel(cpuThread) == 0) {
                break;
            }
        }
    }
    return NULL;
}

void *ioTask(void *args) {
    return NULL;
}

void sut_init() {
    
    if (pthread_mutex_init(&addedFirstElement, NULL) != 0) {
        printf("\n mutex init has failed\n");
        exit(1);
    }
    
    pthread_mutex_lock(&addedFirstElement);
    pthread_mutex_lock(&exitCall);
    
    cpuQueue = queue_create();
    queue_init(&cpuQueue);
    /**
     * if exit == true, break the while 1 loop
     */
    
    if (pthread_create(&cpuThread, NULL, cpuTask, NULL) != 0) {
        perror("Could not create CPU thread");
        exit(1);
    }

//    if (pthread_create(&ioThread, NULL, ioTask, NULL) !=0){
//        perror("Could not create CPU thread");
//        exit(1);
//    }
}

bool sut_create(sut_task_f fn) {
    struct job *j;
    j = malloc(sizeof(struct job));
    
    getcontext(&j->context);
    
    j->context.uc_stack.ss_sp = j->stack;
    j->context.uc_stack.ss_size = sizeof(j->stack);
    j->context.uc_link = &main;
    makecontext(&j->context, fn, 0);

//    swapcontext(&main,&j->context);

//    printf("job BEFORE %p \n", (void *)&j);
//    printf("node BEFORE %p \n", (void *)&node);
//    printf("context BEFORE %p \n", (void *)&j->context);
    
    struct queue_entry *node = queue_new_node(j);
    queue_insert_tail(&cpuQueue, node);
    
    pthread_mutex_unlock(&addedFirstElement);
    //check for error. if there is, return false.
    return true;
}

void sut_yield() {
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    queue_insert_tail(&cpuQueue, head);
    
    struct job *toSwap = (struct job *) (head->data);
    swapcontext(&toSwap->context, &main);
}

void sut_exit() {
    //send shutdown signal
    pthread_kill(cpuThread, 0);
    pthread_join(cpuThread, NULL);
    
}

void sut_open(char *dest, int port) {

}

void sut_write(char *buf, int size) {

}

void sut_close() {

}

char *sut_read() {
    return NULL;
}