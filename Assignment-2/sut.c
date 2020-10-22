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
int taskCounter;
pthread_spinlock_t isCpuDone;

struct task {
    int taskId;
    char *taskStack;
    void *taskFunction;
    ucontext_t context;
};

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
            pthread_spin_unlock(&isCpuDone);//todo break only if IO queue is empty
            // dont know if it will block here if there are no tasks though.
            break;
        }
        usleep(100);
    }
    pthread_exit(NULL);
}

void *ioExecutor(void *args) {
    return NULL;
}

void sut_init() {
    taskCounter = 0;
    pthread_spin_init(&isCpuDone, 0);
    pthread_spin_lock(&isCpuDone);
    
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
//    printf("Library initialized\n");
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
    
    struct queue_entry *node = queue_new_node(t);
    queue_insert_tail(&cpuQueue, node);
//    printf("Task Created\n");
    
    //check for error. if there is, return false.
    return true;
}

void sut_yield() {
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    queue_insert_tail(&cpuQueue, head);
    
    struct task *toSwap = (struct task *) (head->data);
    swapcontext(&toSwap->context, &main);
}

//only difference is that when a task calls exit, we dont add it to the queue
void sut_exit() {
    struct queue_entry *head = queue_pop_head(&cpuQueue);
    struct task *toSwap = (struct task *) (head->data);
    swapcontext(&toSwap->context, &main);
}

void sut_shutdown() {
    //wait until there are no more elements in queue.
    // send shutdown signal to thread
    pthread_spin_lock(&isCpuDone);
    pthread_cancel(cpuThread);
    printf("CPU thread Terminated\n");
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