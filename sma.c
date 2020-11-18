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
//	TODO: Change the Header size if required
#define HEADER_SIZE (2*sizeof(int)) // Size of the Header in a free memory block
//	TODO: Add constants here

typedef enum //	Policy type definition
{
    WORST,
    NEXT
} Policy;

char *sma_malloc_error;
int *startOfMemory = NULL;              //	The pointer to the HEAD of the doubly linked free memory list
int *endOfMemory = NULL;              //	The pointer to the TAIL of the doubly linked free memory list
int *lastMemory = NULL;
unsigned long totalAllocatedSize = 0; //	Total Allocated memory in Bytes
unsigned long totalFreeSize = 0;      //	Total Free memory in Bytes in the free memory list
Policy currentPolicy = WORST;          //	Current Policy
//	TODO: Add any global variables here
char debug[100];

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

void newTag(int *ptr, int sizeOfMemory, int isFree) {
    setSizeOfMemory(ptr, sizeOfMemory);
    setIsMemoryFree(ptr, isFree);
}


//int *getPreviousMemoryLocation(int *ptr){
//    int *itr = startOfMemory;
//    while (itr != endOfMemory){
//        if (getNextMemoryLocation(itr) == ptr){
//            return itr;
//        }
//        itr = getNextMemoryLocation(itr);
//    }
//    return NULL;
//}

/*
 * =====================================================================================
 *	Public Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: sma_malloc
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates a memory block of input size from the heap, and returns a 
 * 					pointer pointing to it. Returns NULL if failed and sets a global error.
 */
int i = 0;

void *sma_malloc(int size) {
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
    }
    
    // Validates memory allocation
    if (pMemory < 0 || pMemory == NULL) {
        sma_malloc_error = "Error: Memory allocation failed!";
        return NULL;
    }
    
    // Updates SMA Info
    totalAllocatedSize += size;
    
    return pMemory;
}

/*
 *	Funcation Name: sma_free
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Deallocates the memory block pointed by the input pointer
 */
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
    }
}

/*
 *	Funcation Name: sma_mallopt
 *	Input type:		int
 * 	Output type:	void
 * 	Description:	Specifies the memory allocation policy
 */
void sma_mallopt(int policy) {
    // Assigns the appropriate Policy
    if (policy == 1) {
        currentPolicy = WORST;
    } else if (policy == 2) {
        currentPolicy = NEXT;
    }
}

/*
 *	Funcation Name: sma_mallinfo
 *	Input type:		void
 * 	Output type:	void
 * 	Description:	Prints statistics about current memory allocation by SMA.
 */
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

/*
 *	Funcation Name: sma_realloc
 *	Input type:		void*, int
 * 	Output type:	void*
 * 	Description:	Reallocates memory pointed to by the input pointer by resizing the
 * 					memory block according to the input size.
 */
void *sma_realloc(void *ptr, int size) {
    // TODO: 	Should be similar to sma_malloc, except you need to check if the pointer address
    //			had been previously allocated.
    // Hint:	Check if you need to expand or contract the memory. If new size is smaller, then
    //			chop off the current allocated memory and add to the free list. If new size is bigger
    //			then check if there is sufficient adjacent free space to expand, otherwise find a new block
    //			like sma_malloc.
    //			Should not accept a NULL pointer, and the size should be greater than 0.
    int *mem = NULL;
    if (size < 0 || ptr == NULL){
        return mem;
    }


    if(getSizeOfMemory(ptr) > size) {
        //assign it
    }
    else {
       mem =  sma_malloc(size);
    }
    return mem;
}

/*
 * =====================================================================================
 *	Private Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: allocate_pBrk
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory by increasing the Program Break
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
//    allocate_block(newBlock, size, excessSize, 0);
    
    return newBlock;
}

/*
 *	Funcation Name: allocate_freeList
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory from the free memory list
 */
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

/*
 *	Funcation Name: allocate_worst_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Worst Fit from the free memory list
 */
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
        newTag(newChunk, largestBlock - HEADER_SIZE - size, 1);
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

/*
 *	Funcation Name: allocate_next_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Next Fit from the free memory list
 */
void *allocate_next_fit(int size) {
    int *nextBlock = NULL;
    int blockFound = 0;

    if (lastMemory == NULL) {
        lastMemory = startOfMemory;
    }

    int *itr = lastMemory;
    while (1) {

        if (getSizeOfMemory(itr) > size && getIsMemoryFree(itr)) {
            nextBlock = itr;
            lastMemory = itr;
            blockFound = 1;
            break;
        }

        if (itr > endOfMemory) {
            itr = startOfMemory;
        }
        itr = getNextMemoryLocation(itr);
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

        if (excess >10) {
            newTag(nextBlock, size, 0);
            newTag(getNextMemoryLocation(nextBlock), excess,1);
        } else {
            newTag(nextBlock, size+8, 0);
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

/*
 *	Funcation Name: allocate_block
 *	Input type:		void*, int, int, int
 * 	Output type:	void
 * 	Description:	Performs routine operations for allocating a memory block
 */
void allocate_block(int *newBlock, int size, int excessSize, int fromFreeList) {
    int *excessFreeBlock; //	pointer for any excess free block
    int addFreeBlock;
    
    // 	Checks if excess free size is big enough to be added to the free memory list
    //	Helps to reduce external fragmentation
    
    //	TODO: Adjust the condition based on your Head and Tail size (depends on your TAG system)
    //	Hint: Might want to have a minimum size greater than the Head/Tail sizes
    addFreeBlock = excessSize > HEADER_SIZE;
    
    //	If excess free size is big enough
    if (addFreeBlock) {
        //	TODO: Create a free block using the excess memory size, then assign it to the Excess Free Block
        
        excessFreeBlock = getNextMemoryLocation(newBlock);
        newTag(excessFreeBlock, size, 1);

//        sprintf(debug, "old location %p \nstart %p \nlocation %p, size %d isFree %d", newBlock,startOfMemory,excessFreeBlock, getSizeOfMemory(excessFreeBlock), getIsMemoryFree(excessFreeBlock));
//        puts(debug);
        
        //	Checks if the new block was allocated from the free memory list
        if (fromFreeList) {
            //	Removes new block and adds the excess free block to the free list
            replace_block_freeList(newBlock, excessFreeBlock);
        } else {
            //	Adds excess free block to the free list
            add_block_freeList(excessFreeBlock);
        }
    }
        //	Otherwise add the excess memory to the new block
    else {
        //	TODO: Add excessSize to size and assign it to the new Block
        
        //	Checks if the new block was allocated from the free memory list
        if (fromFreeList) {
            //	Removes the new block from the free list
            remove_block_freeList(newBlock);
        }
    }
}

/*
 *	Funcation Name: replace_block_freeList
 *	Input type:		void*, void*
 * 	Output type:	void
 * 	Description:	Replaces old block with the new block in the free list
 */
void replace_block_freeList(void *oldBlock, void *newBlock) {
    //	TODO: Replace the old block with the new block
    
    //	Updates SMA info
    totalAllocatedSize += (getSizeOfMemory(oldBlock) - getSizeOfMemory(newBlock));
    totalFreeSize += (getSizeOfMemory(newBlock) - getSizeOfMemory(oldBlock));
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
            setSizeOfMemory(itr, currentBlockSize + nextBlockSize + HEADER_SIZE);
        }
        if (itr >= endOfMemory) {
            break;
        }
        itr = getNextMemoryLocation(itr);

    }
    if (getSizeOfMemory(itr) > MAX_TOP_FREE){
        sbrk(MAX_TOP_FREE - getSizeOfMemory(itr));
    }

}

/*
 *	Funcation Name: add_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Adds a memory block to the the free memory list
 */
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
}

/*
 *	Funcation Name: remove_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Removes a memory block from the the free memory list
 */
void remove_block_freeList(void *block) {
    //	TODO: 	Remove the block from the free list
    //	Hint: 	You need to update the pointers in the free blocks before and after this block.
    //			You also need to remove any TAG in the free block.
    
    setIsMemoryFree(block, 0);
    //	Updates SMA info
    totalAllocatedSize += getSizeOfMemory(block);
    totalFreeSize -= getSizeOfMemory(block);
}


/*
 *	Funcation Name: get_largest_freeBlock
 *	Input type:		void
 * 	Output type:	int
 * 	Description:	Extracts the largest Block Size
 */
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
        if (itr >= endOfMemory) {
            break;
        }

        itr = getNextMemoryLocation(itr);
        numberBlocks++;
    }
    
    return numberBlocks;
}