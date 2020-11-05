#ifndef __SUT_H__
#define __SUT_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ucontext.h>
#include <ucontext.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>

typedef struct TCB {
    int threadid;
    char *threadstack; //pointer to the stack
    void *threadfunc;    //pointer to function
    ucontext_t threadcontext;
} TCB;

typedef struct IOB {
    char *dest;
    int port;
    char *buf;
    bool open;
    int instruction;
    int size;
    TCB *c_task;
} IOB;

typedef void (*sut_task_f)();

void *c_exec();

void *i_exec();

int connect_to_server(const char *host, uint16_t port, int *sockfd);


void sut_init();

bool sut_create(sut_task_f fn);

void sut_yield();

void sut_exit();

void sut_open(char *dest, int port);

void sut_write(char *buf, int size);

void sut_close();

char *sut_read();

void sut_shutdown();

void printout_q();

#endif
