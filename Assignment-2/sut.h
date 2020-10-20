#ifndef SUT_H
#define SUT_H

#include <stdbool.h>

typedef void (*sut_task_f)();

void sut_init();

bool sut_create(sut_task_f fn);

void sut_yield();

void sut_exit();

void sut_open(char *dest, int port);

void sut_write(char *buf, int size);

void sut_close();

char *sut_read();


#endif
