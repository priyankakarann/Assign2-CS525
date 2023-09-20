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
// Buffer Manager checks whether the page is already in cache (If condition)

// Replacement strategies
// LRU
// FIFO
// Clock

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		const int numPages, ReplacementStrategy strategy,
		void *stratData){
    return RC_OK;
};


RC shutdownBufferPool(BM_BufferPool *const bm){
    return RC_OK;
};


RC forceFlushPool(BM_BufferPool *const bm){
    return RC_OK;
};

// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
    return RC_OK;
};

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
    return RC_OK;
};

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
    return RC_OK;
};

//This function will helps to request pages identified by their position in the
//(page number) to be loaded in a page frame
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
		const PageNumber pageNum){
    return RC_OK;
};

// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm){

};

bool *getDirtyFlags (BM_BufferPool *const bm){

};

int *getFixCounts (BM_BufferPool *const bm){

};

int getNumReadIO (BM_BufferPool *const bm){

};

int getNumWriteIO (BM_BufferPool *const bm){

};