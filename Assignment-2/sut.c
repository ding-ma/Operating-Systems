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

struct queue cpuQueue;
struct queue ioQueue;
static ucontext_t main;

struct job {
    ucontext_t context;
    char stack[16 * 1024];
};

void *cpuTask() {
    while (1) {
        struct queue_entry *next = queue_peek_front(&cpuQueue);
        if (next != NULL) {
            struct job *toExecute = (struct job *) (next->data);
            swapcontext(&main, &toExecute->context);
        } else {
            pthread_testcancel();
//            printf("1212");
        }
    }
}

void *ioTask() {

}

void sut_init() {
    cpuQueue = queue_create();
    queue_init(&cpuQueue);
    /**
     * if exit == true, break the while 1 loop
     */
//    pthread_t cpuThread, ioThread;
//    if (pthread_create(&cpuThread, NULL, cpuTask, NULL) !=0){
//        perror("Could not create CPU thread");
//    }
//    pthread_join(cpuThread, NULL);
//
//    pthread_create(&ioThread, NULL, ioTask, NULL);
//    pthread_join(ioThread, NULL);
}

bool sut_create(sut_task_f fn) {
    printf("creating job");
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
    while (1) {
        struct queue_entry *next = queue_peek_front(&cpuQueue);
        if (next != NULL) {
            struct job *toExecute = (struct job *) (next->data);
            swapcontext(&main, &toExecute->context);
        } else {
            break;
//            printf("1212");
        }
    }
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