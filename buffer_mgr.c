#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "buffer_mgr.h"
// #include "replacement_mgr_strat.h"
#include "storage_mgr.h"
#include "buffer_mgr_stat.h"

/*
Task is to maintain an array which is called as Buffer Pool to contain the pages as a cache in the buffer pool
which is fetched from the disk and stored in the memory
Page Table: Is a hashmap which keeps track of pages that currently in the memory
*/

//  int main()
// {
//     return 0;
//  }

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

int noOfPagesRead = 0;
int noOfPagesWrite = 0;
int maxBufferSize = 0;
int rearIndex = 0;
int hit = 0;
int clockPointer = 0;


void FIFO(BM_BufferPool *const bm, Frame *page)
{
	Frame *frame = (Frame *) bm->mgmtData;
	
	int frontIndex = rearIndex % maxBufferSize;

	for(int i = 0; i < maxBufferSize; i++)
	{
		if(frame[frontIndex].fixCount == 0)
		{
			if(frame[frontIndex].dirtyCount == 1)
			{
				SM_FileHandle fHandle;
				openPageFile(bm->pageFile, &fHandle);
				writeBlock((frame[frontIndex].bm_PageHandle).pageNum, &fHandle, frame[frontIndex].smp);
				
				noOfPagesWrite++;
			}
			
			frame[frontIndex].smp = page->smp;
			(frame[frontIndex].bm_PageHandle).pageNum = (page->bm_PageHandle).pageNum;
			frame[frontIndex].dirtyCount = page->dirtyCount;
			frame[frontIndex].fixCount = page->fixCount;
			break;
		}
		else
		{
			frontIndex += 1;
            if (frontIndex % maxBufferSize == 0){
                frontIndex = 0;
            }
		}
	}
}



// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData)
{
    RC return_code = RC_OK;
    if (bm != NULL)
    {
        bm->numPages = numPages;
        bm->pageFile = (char*)pageFileName;
        bm->strategy = strategy;
    }
    else
    {
        RC_message = "The buffer pool was not initialized";
        return_code = RC_BUFFER_NOT_INIT;
        printError(*RC_message);

        return return_code;
    }

    maxBufferSize = numPages;

    Frame *frame = (Frame *)malloc(sizeof(Frame) * numPages);

    if (frame == NULL)
    {
        RC_message = "The memory allocation was not initialized";
        return_code = RC_BUFFER_NOT_INIT;
        printError(*RC_message);

        return return_code;
    }

    for (int i = 0; i < maxBufferSize; i++)
    {
        frame[i].dirtyCount = 0;
        frame[i].fixCount = 0;
        (frame[i].bm_PageHandle).pageNum = 0;
        frame[i].smp = NULL;
    }

    bm->mgmtData = frame;
    printf("BUFFER WAS INITIALIZED SUCCESSFULLY");
    return return_code;
};

// Check if any pages are pinned or there are any dirty pages, then it needs to be resolved first before shutting down the buffer manager
RC shutdownBufferPool(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;

    Frame *frame = (Frame *)bm->mgmtData;

    forceFlushPool(bm);

    for (int i = 0; i < maxBufferSize; i++)
    {
        if (frame[i].fixCount != 0)
        {
            return RC_PINNED_PAGES_ERROR;
        }
    }

    free(frame);
    // free(bm);

    return return_code;
};

// To write all the dirty pages to the disk
RC forceFlushPool(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;
    Frame *frame = (Frame *)bm->mgmtData;

    for (int i = 0; i < maxBufferSize; i++)
    {
        if (frame[i].fixCount == 0 && frame[i].dirtyCount == 1)
        {
            SM_FileHandle fHandle;

            openPageFile(bm->pageFile, &fHandle);
            //!Edge Case
            writeBlock((frame[i].bm_PageHandle).pageNum, &fHandle, frame[i].smp);

            frame[i].dirtyCount = 0;

            noOfPagesWrite += 1;
        }
    }

    return return_code;
};

// To inform the buffer manager that the page is modified by the user when fetched from the buffer, such as the manager needs to update it on the disk as well
// Buffer Manager Interface Access Pages
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    RC return_code = RC_OK;
    Frame *frame = (Frame *)bm->mgmtData;

    for (int i = 0; i < maxBufferSize; i++)
    {
        if ((frame[i].bm_PageHandle).pageNum == page->pageNum)
        {
            frame[i].dirtyCount = 1;
            break;
        }
    }

    return return_code;
};

/*
Once the user is done with the reading or writing, it needs to inform the manager that the page is no longer needed,
this is called as unpinning.
*/
// Decreases the pin count by 1
RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    RC return_code = RC_OK;
    Frame *frame = (Frame *)bm->mgmtData;

    for (int i = 0; i < maxBufferSize; i++)
    {
        if ((frame[i].bm_PageHandle).pageNum == page->pageNum)
        {
            frame[i].fixCount -= 1;
            break;
        }
    }

    return return_code;
};

RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    RC return_code = RC_OK;
    Frame *frame = (Frame *)bm->mgmtData;

    for (int i = 0; i < maxBufferSize; i++)
    {
        if ((frame[i].bm_PageHandle).pageNum == page->pageNum)
        {
            SM_FileHandle fHandle;
            openPageFile(bm->pageFile, &fHandle);
            int didWrite = writeBlock((frame[i].bm_PageHandle).pageNum, &fHandle, frame[i].smp);

            if (didWrite != 0)
            {
                //! Print error messages according to the error code (Use switch Statement)
                return_code = RC_WRITE_FAILED;

                return return_code;
            }
        }
    }

    return return_code;
};

// This function will helps to request pages identified by their position in the
//(page number) to be loaded in a page frame
//  Increases the pin count by 1

RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
    Frame *frame = (Frame *)bm->mgmtData;
    // printPoolContent(bm);
    // Checking if buffer pool is empty and this is the first page to be pinned
    if ((frame[0].bm_PageHandle).pageNum == NO_PAGE) // Use NO_PAGE constant for uninitialized values
    {
        SM_FileHandle fHandle; // Declare the file handle
        RC return_code = RC_OK; // Declare a return code variable

        // Open the page file and handle potential errors
        if(openPageFile(bm->pageFile, &fHandle) != RC_OK){
			printf("AKFJSKLAJSFKLJASKD");
		}
        // Allocate memory for the new page
        frame[0].smp = (SM_PageHandle)malloc(PAGE_SIZE);
        // ensureCapacity(pageNum, &fHandle);
        // Ensure the page exists in the file
        if (pageNum >= fHandle.totalNumPages)
        {
            return RC_IM_KEY_NOT_FOUND;
        }

        // Read the page into memory
        if(readBlock(pageNum, &fHandle, frame[0].smp) != RC_OK){
			printf("mnbmnbmnbnmmnmn");
		}
        // Initialize page frame properties
        (frame[0].bm_PageHandle).pageNum = pageNum;
        frame[0].fixCount += 1;
        rearIndex = 0;
        page->pageNum = pageNum;
        page->data = frame[0].smp;
        
        return RC_OK;
    }
    else
    {
        bool isBufferFull = true;

        for (int i = 0; i < maxBufferSize; i++)
        {
            if ((frame[i].bm_PageHandle).pageNum != NO_PAGE)
            {
                // Checking if page is in memory
                if ((frame[i].bm_PageHandle).pageNum == pageNum)
                {
                    // Increasing fixCount i.e., now there is one more client accessing this page
                    frame[i].fixCount += 1;
                    isBufferFull = false;
                    hit++; // Incrementing hit (hit is used by LRU algorithm to determine the least recently used page)

                    page->pageNum = pageNum;
                    page->data = frame[i].smp;

                    clockPointer++;
                    break;
                }
            }
            else
            {
                SM_FileHandle fHandle;
                RC return_code = RC_OK;

                // Open the page file and handle potential errors
                if(openPageFile(bm->pageFile, &fHandle) != RC_OK){
					printf("XBNMZCBZMNXBCMNZXCMN");
				}

                // Allocate memory for the new page
                frame[i].smp = (SM_PageHandle)malloc(PAGE_SIZE);

                // Ensure the page exists in the file
                if (pageNum >= fHandle.totalNumPages)
                {
                    return RC_IM_KEY_NOT_FOUND;
                }

                // Read the page into memory
                if(readBlock(pageNum, &fHandle, frame[i].smp) != RC_OK){
					printf("NMXBCZMNBXCNMZBNMXCBMNZXBCNMX");
				}

                // Initialize page frame properties
                (frame[i].bm_PageHandle).pageNum = pageNum;
                frame[i].fixCount = 1;
                rearIndex++;
                hit++; // Incrementing hit (hit is used by LRU algorithm to determine the least recently used page)

                page->pageNum = pageNum;
                page->data = frame[i].smp;

                isBufferFull = false;
                break;
            }
        }

        // If isBufferFull = true, then it means that the buffer is full and we must replace an existing page using the page replacement strategy
        if (isBufferFull)
        {
            Frame *newPage = (Frame *) malloc(sizeof(Frame));		
			
			// Reading page from disk and initializing page frame's content in the buffer pool
			SM_FileHandle fHandle;
			openPageFile(bm->pageFile, &fHandle);
			newPage->smp = (SM_PageHandle) malloc(PAGE_SIZE);
			if (readBlock(pageNum, &fHandle, newPage->smp) != RC_OK){
                return RC_READ_NON_EXISTING_PAGE;
            };
			(newPage->bm_PageHandle).pageNum = pageNum;
			newPage->dirtyCount = 0;		
			newPage->fixCount = 1;
			rearIndex++;
			hit++;

			page->pageNum = pageNum;
			page->data = newPage->smp;			

			// Call appropriate algorithm's function depending on the page replacement strategy selected (passed through parameters)
			switch(bm->strategy)
			{			
				case RS_FIFO: // Using FIFO algorithm
					FIFO(bm, newPage);
					break;
				default:
					printf("\nAlgorithm Not Implemented\n");
					break;
			}
        }
    }

    return RC_OK;
}

// RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
//            const PageNumber pageNum)
// {
//     Frame *pageFrame = (Frame *)bm->mgmtData;
	
// 	// Checking if buffer pool is empty and this is the first page to be pinned
// 	if(pageFrame[0].bm_PageHandle.pageNum == -1)
// 	{
// 		// Reading page from disk and initializing page frame's content in the buffer pool
// 		SM_FileHandle *fHandle;
// 		openPageFile(bm->pageFile, fHandle);
// 		pageFrame[0].smp = (SM_PageHandle) calloc(maxBufferSize, sizeof(char));
// 		// ensureCapacity(pageNum, fHandle);
// 		readBlock(pageNum, fHandle, pageFrame[0].smp);
// 		pageFrame[0].bm_PageHandle.pageNum = pageNum;
// 		pageFrame[0].fixCount++;
// 		rearIndex = 0;
// 		page->pageNum = pageNum;
// 		page->data = pageFrame[0].smp;
		
// 		return RC_OK;		
// 	}
// 	else
// 	{
// 		bool isBufferFull = true;
        
// 		for(int i = 0; i < maxBufferSize; i++)
// 		{
// 			if(pageFrame[i].bm_PageHandle.pageNum != -1)
// 			{	
// 				// Checking if page is in memory
// 				if(pageFrame[i].bm_PageHandle.pageNum == pageNum)
// 				{
// 					// Increasing fixCount i.e. now there is one more client accessing this page
// 					pageFrame[i].fixCount++;
// 					isBufferFull = false;
// 					hit++; // Incrementing hit (hit is used by LRU algorithm to determine the least recently used page)

// 					page->pageNum = pageNum;
// 					page->data = pageFrame[i].smp;

// 					clockPointer++;
// 					break;
// 				}				
// 			} else {
// 				SM_FileHandle *fHandle;
// 				openPageFile(bm->pageFile, fHandle);
// 				pageFrame[i].smp = (SM_PageHandle) calloc(maxBufferSize, sizeof(char));
// 				readBlock(pageNum, fHandle, pageFrame[i].smp);
// 				pageFrame[i].bm_PageHandle.pageNum = pageNum;
// 				pageFrame[i].fixCount = 1;
// 				rearIndex++;	
// 				hit++; // Incrementing hit (hit is used by LRU algorithm to determine the least recently used page)

// 				page->pageNum = pageNum;
// 				page->data = pageFrame[i].smp;
				
// 				isBufferFull = false;
// 				break;
// 			}
// 		}
		
// 		// If isBufferFull = true, then it means that the buffer is full and we must replace an existing page using page replacement strategy
// 		if(isBufferFull == true)
// 		{
// 			// Create a new page to store data read from the file.
// 			Frame *newPage = (Frame *) calloc(maxBufferSize, sizeof(Frame));		
			
// 			// Reading page from disk and initializing page frame's content in the buffer pool
// 			SM_FileHandle fh;
// 			openPageFile(bm->pageFile, &fh);
// 			newPage->smp = (SM_PageHandle) calloc(maxBufferSize, sizeof(char));
// 			readBlock(pageNum, &fh, newPage->smp);
// 			newPage->bm_PageHandle.pageNum = pageNum;
// 			newPage->dirtyCount = 0;		
// 			newPage->fixCount = 1;
// 			rearIndex++;
// 			hit++;

// 			page->pageNum = pageNum;
// 			page->data = newPage->smp;			

// 			// Call appropriate algorithm's function depending on the page replacement strategy selected (passed through parameters)
// 			switch(bm->strategy)
// 			{			
// 				case RS_FIFO: // Using FIFO algorithm
// 					FIFO(bm, newPage, rearIndex, maxBufferSize, noOfPagesWrite);
// 					break;
				
// 				default:
// 					printf("\nAlgorithm Not Implemented\n");
// 					break;
// 			}
						
// 		}		
// 		return RC_OK;
// 	}	
// };

// Statistics Interface

PageNumber *getFrameContents(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;
    PageNumber *frameContents = (PageNumber *)malloc(maxBufferSize * sizeof(PageNumber));

    if (frameContents == NULL)
    {
        // Handle memory allocation failure
        RC_message = "The memory allocation was not initialized";
        return_code = RC_BUFFER_NOT_INIT;
        printError(*RC_message);
    }
    else
    {
        Frame *pageFrame = (Frame *)bm->mgmtData;

        for (int i = 0; i < maxBufferSize; i++)
        {
            if ((pageFrame[i].bm_PageHandle).pageNum != -1)
            {
                frameContents[i] = (pageFrame[i].bm_PageHandle).pageNum;
            }
            else
            {
                frameContents[i] = NO_PAGE;
            }
        }
    }

    return frameContents;
};

bool *getDirtyFlags(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;
    bool *dirtyFlagCounts = (bool *)malloc(maxBufferSize * sizeof(bool));

    if (dirtyFlagCounts == NULL)
    {
        // Handle memory allocation failure
        RC_message = "The memory allocation was not initialized";
        return_code = RC_BUFFER_NOT_INIT;
        printError(*RC_message);
    } else {
        Frame *pageFrame = (Frame *)bm->mgmtData;

        for (int i = 0; i < maxBufferSize; i++)
        {
            if (pageFrame[i].dirtyCount == 1)
            {
                dirtyFlagCounts[i] = true;
            }
            else
            {
                dirtyFlagCounts[i] = false;
            }
        }

    }

    return dirtyFlagCounts;
};

// Return the number of clients that has pinned the same page
int *getFixCounts(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;
    int *fixCounts = (int *)malloc(maxBufferSize * sizeof(int));

    if (fixCounts == NULL)
    {
        // Handle memory allocation failure
        RC_message = "The memory allocation was not initialized";
        return_code = RC_BUFFER_NOT_INIT;
        printError(*RC_message);
    } else {
        Frame *frame = (Frame *)bm->mgmtData;

        for (int i = 0; i < maxBufferSize; i++)
        {
            if (frame[i].fixCount != -1)
            {
                fixCounts[i] = frame[i].fixCount;
            }
            else
            {
                fixCounts[i] = NO_PAGE;
            }
        }
    }

    return fixCounts;
};

int getNumReadIO(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;
    if (noOfPagesRead == NULL)
    {
        RC_message = "The variable is not defined";
        return_code = RC_IM_KEY_NOT_FOUND;
        printError(*RC_message);

        return return_code;
    }

    return noOfPagesRead;
};

int getNumWriteIO(BM_BufferPool *const bm)
{
    RC return_code = RC_OK;
    if (noOfPagesWrite == NULL)
    {
        RC_message = "The variable is not defined";
        return_code = RC_IM_KEY_NOT_FOUND;
        printError(*RC_message);

        return return_code;
    }

    return noOfPagesWrite;
};

/*
Make the buffer pool functions thread safe. This extension would result in your buffer manager being closer to real life buffer manager implementations.
Implement additional page replacement strategies such as CLOCK or LRU-k.
*/