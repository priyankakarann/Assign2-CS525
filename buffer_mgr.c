#include <stdio.h>
#include <string.h>

#include "buffer_mgr.h"

/*
Task is to maintain an array which is called as Buffer Pool to contain the pages as a cache in the buffer pool
which is fetched from the disk and stored in the memory
Page Table: Is a hashmap which keeps track of pages that currently in the memory
*/

int main(){
    return 0;
}

// Returns the page file by the position of the page file
// A buffer pool consists of a fixed number of pages called as frames to store the pages from the disk
// Buffer Manager checks whether the page is already in cache (Buffer pool) (If condition) 

/*
If the page already exists in the buffer pool then it returns the pointer for the page
If not, then the page is loaded to the appropriate frame in the buffer pool which is done by the replacement strategy
Once, the frame is found, the page is loaded and the pointer is returned to the user
*/

// Replacement strategies
// LRU
// FIFO
// Clock
// LFU
// LRU-k


int maxBufferSize = 0;
int noOfPagesRead = 0;
int noOfPagesWrite = 0;


// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		const int numPages, ReplacementStrategy strategy,
		void *stratData){
    return RC_OK;
};

// Check if any pages are pinned or there are any dirty pages, then it needs to be resolved first before shutting down the buffer manager
RC shutdownBufferPool(BM_BufferPool *const bm){
    return RC_OK;
};

// To write all the dirty pages to the disk
RC forceFlushPool(BM_BufferPool *const bm){
    return RC_OK;
};

// To inform the buffer manager that the page is modified by the user when fetched from the buffer, such as the manager needs to update it on the disk as well
// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
    return RC_OK;
};


/*
Once the user is done with the reading or writing, it needs to inform the manager that the page is no longer needed,
this is called as unpinning.
*/
// Decreases the pin count by 1
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
    return RC_OK;
};

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
    return RC_OK;
};

//This function will helps to request pages identified by their position in the
//(page number) to be loaded in a page frame
// Increases the pin count by 1
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
		const PageNumber pageNum){
    return RC_OK;
};

// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm){

};

bool *getDirtyFlags (BM_BufferPool *const bm){

};

// Return the number of clients that has pinned the same page
int *getFixCounts (BM_BufferPool *const bm){

};

int getNumReadIO (BM_BufferPool *const bm){

};

int getNumWriteIO (BM_BufferPool *const bm){

};

/*
Make the buffer pool functions thread safe. This extension would result in your buffer manager being closer to real life buffer manager implementations.
Implement additional page replacement strategies such as CLOCK or LRU-k.
*/