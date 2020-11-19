/*
 * =====================================================================================
 *
 *  Filename:  		sma.h
 *
 *  Description:	Header file for SMA.
 *
 *  Version:  		1.0
 *  Created:  		3/11/2020 9:30:00 AM
 *  Revised:  		-
 *  Compiler:  		gcc
 *
 *  Author:  		Mohammad Mushfiqur Rahman
 *      
 *  Instructions:   Please address all the "TODO"s in the code below and modify them
 *                  accordingly. Refer to the Assignment Handout for further info.
 * =====================================================================================
 */

/* Includes */
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//  Policies definition
#define WORST_FIT    1
#define NEXT_FIT    2

/**
 * Any error will be sprinted here.
 */
extern char sma_malloc_error[100];

//  Public Functions declaration
/**
 * Allocates a memory block of input size from the heap
 * @param size of request
 * @return ptr to allocated memory, NULL if failed and set a global error
 */
void *sma_malloc(int size);

/**
 * Deallocates the memory block pointed by the input pointer
 * @param ptr to the chunk of memory
 */
void sma_free(void *ptr);

/**
 * Specifies the memory allocation policy
 * @param policy 1 for worse fit, 2 for next fit
 */
void sma_mallopt(int policy);

/**
 * Prints statistics about current memory allocation by SMA.
 */
void sma_mallinfo();

/**
 * Reallocates memory pointed to by the input pointer by resizing the memory block according to the input size.
 * @param ptr ptr to memory
 * @param size size we want to allocate
 * @return ptr to allocated memory
 */
void *sma_realloc(void *ptr, int size);


/**
 * Allocates memory by increasing the Program Break. It will also create 128kb more as the free head space
 * @param size to allocate
 * @return ptr to last program break
 */
static void *allocate_pBrk(int size);


/**
 * Allocates memory from the free memory list
 * @param size to allocate
 * @return ptr to block from the free list
 */
static void *allocate_freeList(int size);


/**
 * Allocates memory using Worst Fit from the free memory list
 * @param size to allocate
 * @return ptr to allocated block
 */
static void *allocate_worst_fit(int size);


/**
 * Allocates memory using Next Fit from the free memory list
 * @param size to allocate
 * @return ptr to allocated block
 */
static void *allocate_next_fit(int size);


/**
 * Merges the adjacent free blocks from right to left in the list to create a bigger one
 */
void mergeCells();


/**
 * Adds a memory block to the the free memory list
 * @param block ptr to block
 */
static void add_block_freeList(int *block);


/**
 * Removes a memory block from the the free memory list
 * @param block ptr to block
 */
static void remove_block_freeList(void *block);


/**
 *	Gets largest free block in the list
 *  @return largest Block Size
 */
static int get_largest_freeBlock();


/**
 *
 * @return Iterate Over Memory to pull statistics
 */
void getStats();

/**
 * Clears fragmentation left behind last block (especially the tags)
 */
void clearFragmentation();


/**
 * Gets size of memory given to program
 * @param ptr pointer of malloc memory
 * @return size of that memory chunk
 */
int getSizeOfMemory(int *ptr);


/**
 * Gets size of memory given to program
 * @param ptr pointer of malloc memory
 * @param size of that memory chunk
 */
void setSizeOfMemory(int *ptr, int size);


/**
 * Gets if memory slot is free
 * @param ptr malloc ptr
 * @return 1 if free, 0 if not
 */
int getIsMemoryFree(int *ptr);


/**
 * Sets the memory slot
 * @param ptr malloc ptr
 * @param free 1 if free, 0 if not
 */
void setIsMemoryFree(int *ptr, int free);


/**
 * Gets the next memory location based on the current one
 * Traverses the "linked list"
 * @param ptr of current memory address
 * @return ptr to next memory address
 */
int *getNextMemoryLocation(int *ptr);


/**
 * Creates a tag for a given memory address
 * @param ptr to memory given by malloc
 * @param size of that memory
 * @param isFree 1 if free, 0 if not
 */
void newTag(int *ptr, int size, int isFree);


/**
 * Gets the previous memory location based on the current one
 * This needs to iterate throught the entire list as we have a singly linked list
 * @param ptr current malloc ptr
 * @return ptr to previous memory location
 */
int *getPreviousMemoryLocation(int *ptr);