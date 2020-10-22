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
int taskCounter, currentTask;

struct task {
    int taskId;
    char *taskStack;
    void *taskFunction;
    ucontext_t context;
};

// todo wait for item to be added to list
//  todo wait for shutdown signal

/*
 * if the context of one is finished, we need to remove it from the qeueue. not put it at the end.
 */


void *cpuExecutor(void *args) {
    printf("CPU Thread start\n");
    while (1) {
        struct queue_entry *next = queue_peek_front(&cpuQueue);
        if (next != NULL) {
            struct task *toExecute = (struct task *) (next->data);
            swapcontext(&main, &toExecute->context);
        } else {
//            if (pthread_cancel(cpuThread) == 0) {
//                break;
//            }
        }
        usleep(100);
    }
    return NULL;
}

void *ioExecutor(void *args) {
    return NULL;
}

void sut_init() {
    taskCounter = 0;
    currentTask = 0;
    
    pthread_mutex_lock(&exitCall);
    
    cpuQueue = queue_create();
    queue_init(&cpuQueue);
    if (pthread_create(&cpuThread, NULL, cpuExecutor, NULL) != 0) {
        perror("Could not create CPU thread\n");
        exit(1);
    }

//    ioQueue = queue_create();
//    queue_init(&ioQueue);
//    if (pthread_create(&ioThread, NULL, ioExecutor, NULL) !=0){
//        perror("Could not create CPU thread");
//        exit(1);
//    }
    printf("Library initialized\n");
}

bool sut_create(sut_task_f fn) {
    struct task *t;
    t = malloc(sizeof(struct task));
    
    getcontext(&t->context);
    
    t->taskId = taskCounter;
    t->taskStack = (char *) malloc(STACK_SIZE);
    t->context.uc_stack.ss_sp = t->taskStack;
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = 0;
    t->context.uc_stack.ss_flags = 0;
    t->taskFunction = fn;
    
    makecontext(&(t->context), fn, 1, t);
    
    taskCounter++;
//    swapcontext(&main,&j->context);

//    printf("task BEFORE %p \n", (void *)&j);
//    printf("node BEFORE %p \n", (void *)&node);
//    printf("context BEFORE %p \n", (void *)&j->context);
    
    struct queue_entry *node = queue_new_node(t);
    queue_insert_tail(&cpuQueue, node);
    printf("Task Created\n");
    
    //check for error. if there is, return false.
    return true;
}

void sut_yield() {
    printf("Yielding task \n");
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    queue_insert_tail(&cpuQueue, head);
    
    struct task *toSwap = (struct task *) (head->data);
    swapcontext(&toSwap->context, &main);
}

void sut_exit() {
    //send shutdown signal
//    pthread_kill(cpuThread, 0);
//    pthread_join(cpuThread, NULL);
    
}

void sut_shutdown() {
    while (1) {
    
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