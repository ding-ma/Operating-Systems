#ifndef __SUT_H__
#define __SUT_H__

#include <stdbool.h>

#define STACK_SIZE (1024*64)
#define READSIZE 1024

typedef void (*sut_task_f)();

/**
 * Initializes the SUT library
 */
void sut_init();

/**
 * Creates a task in the CPU queue with the context of the function
 * @param fn function we want to execute
 * @return 0 on failure, 1 on success
 */
bool sut_create(sut_task_f fn);

/**
 * Execute the next task in the Queue
 */
void sut_yield();

/**
 * Exit of a function
 */
void sut_exit();

/**
 * Opens a connection with the server. Those can be change in sut.h
 * Inserts the request at the end of IO queue
 * @param dest hostIp
 * @param port hostPort
 */
void sut_open(char *dest, int port);

/**
 * Writes to Server
 * @param buf message we want to write
 * @param size size of that message
 */
void sut_write(char *buf, int size);

/**
 * Close connection with the server. It will also terminate the server with the simple_server.c implementation
 */
void sut_close();

/**
 * Reads from the server
 * @return message read
 */
char *sut_read();

/**
 * Prepares to close the library after all the tasks are done
 */
void sut_shutdown();


#endif
