/*
 * =====================================================================================
 *
 *	Filename:  		sma.c
 *
 *  Description:	Base code for Assignment 3 for ECSE-427 / COMP-310
 *
 *  Version:  		1.0
 *  Created:  		6/11/2020 9:30:00 AM
 *  Revised:  		-
 *  Compiler:  		gcc
 *
 *  Author:  		Mohammad Mushfiqur Rahman
 *      
 *  Instructions:   Please address all the "TODO"s in the code below and modify 
 * 					them accordingly. Feel free to modify the "PRIVATE" functions.
 * 					Don't modify the "PUBLIC" functions (except the TODO part), unless
 * 					you find a bug! Refer to the Assignment Handout for further info.
 * =====================================================================================
 */

/* Includes */
#include "sma.h" // Please add any libraries you plan to use inside this file

/* Definitions*/
#define ONE_BYTE 1024
#define MAX_TOP_FREE (128 *  ONE_BYTE) // Max top free block size = 128 Kbytes
#define HEADER_SIZE (2*sizeof(int)) // Size of the Header in a free memory block

typedef enum //	Policy type definition
{
    WORST,
    NEXT
} Policy;

char sma_malloc_error[100];
int *startOfMemory = NULL;              //	The pointer to the HEAD of the doubly linked free memory list
int *endOfMemory = NULL;              //	The pointer to the TAIL of the doubly linked free memory list
int *lastMemory = NULL;                 // last memory location of NEXT FIT policy
unsigned long totalAllocatedSize = 0; //	Total Allocated memory in Bytes
unsigned long totalFreeSize = 0;      //	Total Free memory in Bytes in the free memory list
Policy currentPolicy = WORST;          //	Current Policy
char debug[100];

/*
 * =====================================================================================
 *	Public Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: sma_malloc
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:
 */
int i = 0;


void *sma_malloc(int size) {
    if (size < 0) {
        strcpy(sma_malloc_error, "Size cannot be smaller than 0");
        return NULL;
    }
    
    void *pMemory = NULL;
    
    // Checks if the free list is empty
    if (startOfMemory == NULL) {
        // Allocate memory by increasing the Program Break
        sbrk(100);
        pMemory = allocate_pBrk(size);
        i++;
    } else { // If free list is not empty
        
        // Allocate memory from the free memory list
        pMemory = allocate_freeList(size);
        // If a valid memory could NOT be allocated from the free memory list
        if (pMemory == (void *) -2) {
            // Allocate memory by increasing the Program Break
            pMemory = allocate_pBrk(size);
            i++;
        }
        clearFragmentation();
    }
    
    // Validates memory allocation
    if (pMemory < 0 || pMemory == NULL) {
        strcpy(sma_malloc_error, "Error: Memory allocation failed!");
        return NULL;
    }
    
    // Updates SMA Info
    totalAllocatedSize += size;
    
    return pMemory;
}


void sma_free(void *ptr) {
    //	Checks if the ptr is NULL
    if (ptr == NULL) {
        puts("Error: Attempting to free NULL!");
    }
        //	Checks if the ptr is beyond Program Break
    else if (ptr > sbrk(0)) {
        puts("Error: Attempting to free unallocated space!");
    } else {
        //	Adds the block to the free memory list
        add_block_freeList(ptr);
        clearFragmentation();
        
    }
}


void sma_mallopt(int policy) {
    // Assigns the appropriate Policy
    if (policy == 1) {
        currentPolicy = WORST;
    } else if (policy == 2) {
        currentPolicy = NEXT;
    }
}


void sma_mallinfo() {
    //	Finds the largest Contiguous Free Space (should be the largest free block)
    int largestFreeBlock = get_largest_freeBlock();
    int numberOfBlocks = getNumberOfBlocks();
    char str[60];
    
    //	Prints the SMA Stats
    sprintf(str, "Total number of blocks: %d", numberOfBlocks);
    puts(str);
    
    sprintf(str, "Total number of bytes allocated: %lu", totalAllocatedSize);
    puts(str);
    sprintf(str, "Total free space: %lu", totalFreeSize);
    puts(str);
    sprintf(str, "Size of largest contigious free space (in bytes): %d", largestFreeBlock);
    puts(str);
    
    sprintf(debug, "sbrk called number called %d", i);
    puts(debug);
    
}


void *sma_realloc(void *ptr, int size) {
    // TODO: 	Should be similar to sma_malloc, except you need to check if the pointer address
    //			had been previously allocated.
    // Hint:	Check if you need to expand or contract the memory. If new size is smaller, then
    //			chop off the current allocated memory and add to the free list. If new size is bigger
    //			then check if there is sufficient adjacent free space to expand, otherwise find a new block
    //			like sma_malloc.
    //			Should not accept a NULL pointer, and the size should be greater than 0.
    int *mem = NULL;
    if (size < 0 || ptr == NULL) {
        strcpy(sma_malloc_error, "Size cannot be smaller than 0 or pointer cannot be NULL");
        return NULL;
    }
//    int a = getSizeOfMemory(getPreviousMemoryLocation(getNextMemoryLocation(ptr)));
//    int b = getSizeOfMemory(ptr);
    
    if (getSizeOfMemory(ptr) > size) {
        int excess = getSizeOfMemory(ptr);
        if (excess > HEADER_SIZE) {
            setIsMemoryFree(ptr, 0);
        } else {
            newTag(ptr, size, 0);
            remove_block_freeList(ptr);
            newTag(getNextMemoryLocation(ptr), excess, 1);
            add_block_freeList(ptr);
        }
    } else {
        sma_free(ptr);
        mem = sma_malloc(size);
    }
    clearFragmentation();
    return mem;
}

/*
 * =====================================================================================
 *	Private Functions for SMA
 * =====================================================================================
 */


void *allocate_pBrk(int size) {
    int *newBlock = NULL;
    int excessSize;
    
    newBlock = sbrk(size + MAX_TOP_FREE + HEADER_SIZE); // we will always get the maximum size
    excessSize = MAX_TOP_FREE;
    
    //	TODO: 	Allocate memory by incrementing the Program Break by calling sbrk() or brk()
    //	Hint:	Getting an exact "size" of memory might not be the best idea. Why?
    //			Also, if you are getting a larger memory, you need to put the excess in the free list
    
    //	Allocates the Memory Block
    newTag(newBlock, size, 0);
    
    if (startOfMemory == NULL) {
        startOfMemory = newBlock;
    }
    
    endOfMemory = getNextMemoryLocation(newBlock);
    newTag(endOfMemory, excessSize, 1);
    
    return newBlock;
}


void *allocate_freeList(int size) {
    void *pMemory = NULL;
    
    if (currentPolicy == WORST) {
        // Allocates memory using Worst Fit Policy
        pMemory = allocate_worst_fit(size);
    } else if (currentPolicy == NEXT) {
        // Allocates memory using Next Fit Policy
        pMemory = allocate_next_fit(size);
    } else {
        pMemory = NULL;
    }
    
    return pMemory;
}


void *allocate_worst_fit(int size) {
    int *worstBlock = NULL;
    int excessSize;
    int blockFound;
 
    int largestBlock = get_largest_freeBlock();

//    sprintf(debug, "size of worse block %d", largestBlock/ONE_BYTE);
//    puts(debug);
//
    blockFound = largestBlock > size;
    //	TODO: 	Allocate memory by using Worst Fit Policy
    //	Hint:	Start off with the freeListHead and iterate through the entire list to
    //			get the largest block
    
    //	Checks if appropriate block is found.
//    int *itr = startOfMemory;
//    int biggestSize = 0;
//    while (1){
//        if (getSizeOfMemory(itr) > biggestSize){
//            biggestSize = getSizeOfMemory(itr);
//        }
//        itr = getNextMemoryLocation(itr);
//
//        if (itr != endOfMemory){
//            if (getSizeOfMemory(itr) > biggestSize){
//                biggestSize = getSizeOfMemory(itr);
//            }
//            break;
//        }
//    }
//
//    worstBlock = itr;
//    blockFound = biggestSize > size;
//    excessSize = biggestSize - size - HEADER_SIZE;
//
//
//    sprintf(debug, "allocating from free blocks block location %p next block is at %p end %p", worstBlock, getNextMemoryLocation(worstBlock), endOfMemory);
//    puts(debug);
    
    if (blockFound) {
        int *itr = startOfMemory;
        while (1) {
            if (getSizeOfMemory(itr) == largestBlock) {
                break;
            }
            itr = getNextMemoryLocation(itr);
        }
        newTag(itr, size, 0);
        worstBlock = itr;
        
        int *newChunk = getNextMemoryLocation(itr);
        newTag(newChunk, (largestBlock - HEADER_SIZE - size), 1);
        if (newChunk > endOfMemory) {
            endOfMemory = newChunk;
        }
//        newTag(worstBlock, size,0);
        //	Allocates the Memory Block
//        allocate_block(worstBlock, size, excessSize, 1);
    } else {
        //	Assigns invalid address if appropriate block not found in free list
        worstBlock = (void *) -2;
    }
    
    return worstBlock;
}


void *allocate_next_fit(int size) {
    int *nextBlock = NULL;
    int blockFound;
    
    if (lastMemory == NULL) { //set to start if we never used this scheme
        lastMemory = startOfMemory;
    }
    
    int *itr = lastMemory;
    while (1) {
        
        if (getSizeOfMemory(itr) >= size && getIsMemoryFree(itr)) {
            nextBlock = itr;
            lastMemory = itr;
            blockFound = 1;
            break;
        }
        
        if (itr > endOfMemory) {
            itr = startOfMemory;
        }
        
        itr = getNextMemoryLocation(itr);
        
        if (itr == lastMemory) {
            blockFound = 0;
            break;
        }
    }
    
    
    
    //	TODO: 	Allocate memory by using Next Fit Policy
    //	Hint:	You should use a global pointer to keep track of your last allocated memory address, and
    //			allocate free blocks that come after that address (i.e. on top of it). Once you reach
    //			Program Break, you start from the beginning of your heap, as in with the free block with
    //			the smallest address)
    
    //	Checks if appropriate found is found.
    if (blockFound) {
        int sizeOfBlock = getSizeOfMemory(itr);
        int excess = sizeOfBlock - size - HEADER_SIZE;
        
        if (excess > HEADER_SIZE) {
            newTag(nextBlock, size, 0);
            newTag(getNextMemoryLocation(nextBlock), excess, 1);
            if (getNextMemoryLocation(nextBlock) > endOfMemory) {
                endOfMemory= getNextMemoryLocation(nextBlock);
            }
        } else {
            newTag(nextBlock, size + 8, 0);
        }

//        sprintf(debug, "memory size %d, location %p excess %d", getSizeOfMemory(itr), itr, excess);
//        puts(debug);
        
        
        //	Allocates the Memory Block
//        allocate_block(nextBlock, size, excessSize, 1);
    } else {
        //	Assigns invalid address if appropriate block not found in free list
        nextBlock = (void *) -2;
    }
    
    return nextBlock;
}

void mergeCells() {
    int *itr = startOfMemory;
    
    
    while (itr != endOfMemory) {
        //if both adjacent are free we can merge them
//        sprintf(debug, "current %d, next %d, size of curr %d",getIsMemoryFree(itr),getIsMemoryFree(getNextMemoryLocation(itr)),getSizeOfMemory(itr) );
//        puts(debug);
        if (getIsMemoryFree(itr) && getIsMemoryFree(getNextMemoryLocation(itr))) {
            int currentBlockSize = getSizeOfMemory(itr);
            int nextBlockSize = getSizeOfMemory(getNextMemoryLocation(itr));
//            sprintf(debug, "current %d, next %d, next one %d",currentBlockSize,nextBlockSize, currentBlockSize+nextBlockSize);
//            puts(debug);
            setSizeOfMemory(itr, (currentBlockSize + nextBlockSize + HEADER_SIZE));
        }
        if (itr >= endOfMemory) {
            break;
        }
    
        itr = getNextMemoryLocation(itr);
    }
    if (getSizeOfMemory(itr) > MAX_TOP_FREE) {
        sbrk(MAX_TOP_FREE - getSizeOfMemory(itr));
        endOfMemory = sbrk(0);
    }
//    clearFragmentation();
    
}

void add_block_freeList(int *block) {
    //	TODO: 	Add the block to the free list
    //	Hint: 	You could add the free block at the end of the list, but need to check if there
    //			exits a list. You need to add the TAG to the list.
    //			Also, you would need to check if merging with the "adjacent" blocks is possible or not.
    //			Merging would be tideous. Check adjacent blocks, then also check if the merged
    //			block is at the top and is bigger than the largest free block allowed (128kB).
    
    setIsMemoryFree(block, 1);

//    sprintf(debug, "curr location %p end of block %p", block, endOfMemory);
//    puts(debug);
    mergeCells();
    //	Updates SMA info
    totalAllocatedSize -= getSizeOfMemory(block);
    totalFreeSize += getSizeOfMemory(block);
    if(getSizeOfMemory(block) == 16*ONE_BYTE){
        lastMemory = block;
    }
}


void remove_block_freeList(void *block) {
    //	TODO: 	Remove the block from the free list
    //	Hint: 	You need to update the pointers in the free blocks before and after this block.
    //			You also need to remove any TAG in the free block.
    
    setIsMemoryFree(block, 0);
    //	Updates SMA info
    totalAllocatedSize += getSizeOfMemory(block);
    totalFreeSize -= getSizeOfMemory(block);
}


int get_largest_freeBlock() {
    int largestBlockSize = 0;
    int numberBlocks = 0;
    int *itr = startOfMemory;
    while (1) {
        if (getIsMemoryFree(itr) && getSizeOfMemory(itr) > largestBlockSize) {
            largestBlockSize = getSizeOfMemory(itr);
            if (itr == endOfMemory) {
                break;
            }
        }
        itr = getNextMemoryLocation(itr);
        
        if (itr > endOfMemory) {
            break;
        }
        numberBlocks++;
        
    }
    
    return largestBlockSize;
}


int getNumberOfBlocks() {
    int numberBlocks = 0;
    int *itr = startOfMemory;
    while (1) {
        
        numberBlocks++;
        
        if (itr >= endOfMemory) {
            break;
        }
        
        itr = getNextMemoryLocation(itr);
    }
    
    return numberBlocks;
}


void clearFragmentation(){
    int *itr = startOfMemory;
    int *itr_2 = getNextMemoryLocation(itr);
    while (1) {
    
        //get rid of the fragmentation
        if (itr_2 < endOfMemory && getSizeOfMemory(itr_2) <= HEADER_SIZE) {
            newTag(itr,(getSizeOfMemory(itr)+getSizeOfMemory(itr_2)+ HEADER_SIZE),getIsMemoryFree(itr));
            itr_2 = getNextMemoryLocation(itr_2);
            continue;
        }
        
        if (itr_2 >= endOfMemory) {
            break;
        }
        
        itr = getNextMemoryLocation(itr);
        itr_2 = getNextMemoryLocation(itr_2);
    }
}



int getSizeOfMemory(int *ptr) {
    ptr = ptr - 8;
    return *(int *) ptr;
}

void setSizeOfMemory(int *ptr, int size) {
    ptr = ptr - 8;
    *ptr = size;
}

int getIsMemoryFree(int *ptr) {
    ptr = ptr - 4;
    return *(int *) ptr;
}

void setIsMemoryFree(int *ptr, int free) {
    ptr = ptr - 4;
    *ptr = free;
}

int *getNextMemoryLocation(int *ptr) {
    int size = getSizeOfMemory(ptr);
    ptr = (ptr + (size + HEADER_SIZE) / 4);
    return ptr;
}

void newTag(int *ptr, int size, int isFree) {
    setSizeOfMemory(ptr, size);
    setIsMemoryFree(ptr, isFree);
}


int *getPreviousMemoryLocation(int *ptr) {
    int *itr = startOfMemory;
    while (itr != endOfMemory) {
        if (getNextMemoryLocation(itr) == ptr) {
            return itr;
        }
        itr = getNextMemoryLocation(itr);
    }
    return NULL;
}
