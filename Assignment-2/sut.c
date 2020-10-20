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

void sut_init() {
    cpuQueue = queue_create();
    queue_init(&cpuQueue);
    /**
     * if exit == true, break the while 1 loop
     */
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
    struct queue_entry *n = queue_pop_head(&cpuQueue);
    struct job *toExecute = (struct job *) (n->data);
    swapcontext(&main, &toExecute->context); //cant typecast
    
    //check for error. if there is, return false.
    return true;
}

void sut_yield() {
    
    struct queue_entry *main = queue_pop_head(&cpuQueue);
    queue_insert_tail(&cpuQueue, main);
    printf("yield \n");
//    swapcontext( &main);
}

void sut_exit() {
    while (1) {
        struct queue_entry *next = queue_peek_front(&cpuQueue);
        if (next != NULL) {
            struct job *toExecute = (struct job *) next->data;
            printf("job after %p \n", (void *) &toExecute);
            printf("node after %p \n", (void *) &next);
            printf("context after %p \n", (void *) &toExecute->stack);
            
            swapcontext(&main, &toExecute->context);
            break;
//            swapcontext(&main,&j->context);
            printf("here? \n");
        } else {
            printf("empty!");
            break;
        }
        printf("????????? \n");
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