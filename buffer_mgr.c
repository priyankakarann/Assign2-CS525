#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "buffer_mgr.h"
#include "replacement_mgr_strat.h"
#include "storage_mgr.h"

/*
Task is to maintain an array which is called as Buffer Pool to contain the pages as a cache in the buffer pool
which is fetched from the disk and stored in the memory
Page Table: Is a hashmap which keeps track of pages that currently in the memory
*/

//  int main()
// {
//     return 0;
//  }

// Returns the page file by the position of the page
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
int hit = 0;
int clockPointer = 0;

// initializes a buffer pool with arguments for buffer manager,name of page file,number of page in buffer pool,replacement strategy to be used
extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
						 const int numPages, ReplacementStrategy strategy,void *stratData)

// initializing variable return_code of RC type with RC_OK value						 
{
	RC return_code = RC_OK;

	// first checks if pointer bm is not null and then  sets the attributes of the BM_BufferPool structure that it points to
	if (bm != NULL)
	{
		bm->numPages = numPages;
		bm->strategy = strategy;
		bm->pageFile = (char *)pageFileName;
		maxBufferSize = numPages;
	}
	// allocating dynamic memory for an array of frames and storing a pointer to the first frame in the variable frame
	Frame *frame = malloc(numPages * sizeof(Frame));
	int i = 0;

	// using while loop to initialize the individual frames in the buffer pool.
	while (i < maxBufferSize)
	{
		frame[i].bm_PageHandle.data = NULL;
		frame[i].bm_PageHandle.pageNum = -1;
		frame[i].dirtyCount = 0;
		frame[i].fixCount = 0;
		frame[i].hit = 0;

		i += 1;
	}
	// is assigning the mgmtData field of the BM_BufferPool structure pointed to by bm to the memory address pointed to by the frame pointer
	bm->mgmtData = frame;

	return return_code;
}

// defining function to shut down buffer pool
extern RC shutdownBufferPool(BM_BufferPool *const bm)
{
	RC return_code = RC_OK;
	// The code casts bm->mgmtData to a pointer of type Frame* and assigns it to the frame pointer.
	Frame *frame = (Frame *)bm->mgmtData;
	// Calling  forceFlushPool function with the bm as its argument.
	int didPagesWrite = forceFlushPool(bm);

	// If the condition is successful, it checks whether any frames have a non-zero fixCount, indicating that pages are still in use. If pages are in use, it returns an error code.

	if (didPagesWrite == RC_OK)
	{
		int i = 0;
		while (i < maxBufferSize)
		{
			if (frame[i].fixCount != 0)
			{
				return RC_READ_NON_EXISTING_PAGE;
			}
			i += 1;
		}
		free(frame);
	}
	// If the write operation failed, it sets an appropriate error code and prints an error message.
	else
	{
		return_code = RC_WRITE_FAILED;
		printError(return_code);
	}

	return return_code;
}

//initializing forceFlushPool function to ensure that all dirty pages in the buffer pool are saved back to their respective disk locations
extern RC forceFlushPool(BM_BufferPool *const bm)
{
	RC return_code = RC_OK;
	// The code casts bm->mgmtData to a pointer of type Frame* and assigns it to the frame pointer.
	Frame *frame = (Frame *)bm->mgmtData;

	int i = 0;
	// this code iterates through the frames in the buffer pool, checks if a page is dirty and not pinned, and if so, it writes the page to the associated page file on disk.
	while (i < maxBufferSize)
	{
		if (frame[i].dirtyCount == 1 && frame[i].fixCount == 0)
		{
			SM_FileHandle sm_fileHandle;
			openPageFile(bm->pageFile, &sm_fileHandle);

			int didWrite = writeBlock(frame[i].bm_PageHandle.pageNum, &sm_fileHandle, frame[i].bm_PageHandle.data);
			// If the write is successful, the page's dirty flag is cleared
			if (didWrite == RC_OK)
			{
				frame[i].dirtyCount = 0;
				noOfPagesWrite += 1;
			}
			// if there's an error during the write, it returns an error code and prints an error message.
			else
			{
				return_code = RC_WRITE_FAILED;
				printError(return_code);

				return return_code;
			}
		}

		i += 1;
	}

	return return_code;
}

//definingmarkDirty function to mark a page as "dirty." 
extern RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	RC return_code = RC_OK;
	Frame *frame = (Frame *)bm->mgmtData;

	int i = 0;
	// creating a loop that iterates through an array of frames  using variable i to traverse the frames within the array, and maxBufferSize.
	while (i < maxBufferSize)
	{
		if (frame[i].bm_PageHandle.pageNum == page->pageNum)
		{
			frame[i].dirtyCount = 1;
			break;
		}

		i += 1;
	}

	return return_code;
}

//initializing unpinPage function to release a previously pinned page in the buffer pool.
extern RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	RC return_code = RC_OK;
	// The code casts bm->mgmtData to a pointer of type Frame* and assigns it to the frame pointer.
	Frame *frame = (Frame *)bm->mgmtData;

	int i = 0;
	// this loop is used to locate a specific page in the buffer pool frames based on its pageNum and decrement the fixCount for that page.
	while (i < maxBufferSize)
	{
		if (frame[i].bm_PageHandle.pageNum == page->pageNum)
		{
			frame[i].fixCount -= 1;
			break;
		}

		i += 1;
	}

	return return_code;
}

//defining forcePage function to write a page from the buffer pool back to the disk
extern RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	RC return_code = RC_OK;
	// The code casts bm->mgmtData to a pointer of type Frame* and assigns it to the frame pointer.
	Frame *frame = (Frame *)bm->mgmtData;

	int i = 0;
	// The loop continues to iterate through the frames in the buffer pool, checking each frame to see if its page matches the one you want to write
	while (i < maxBufferSize)
	{
		// If a match is found, it attempts to write the page back to the page file.
		if (frame[i].bm_PageHandle.pageNum == page->pageNum)
		{
			SM_FileHandle sm_fileHandle;
			openPageFile(bm->pageFile, &sm_fileHandle);
			int didWrite = writeBlock(frame[i].bm_PageHandle.pageNum, &sm_fileHandle, frame[i].bm_PageHandle.data);

			if (didWrite == RC_OK)
			{
				frame[i].dirtyCount = 0;
				noOfPagesWrite += 1;
			}
			// else returns  an error message
			else
			{
				return_code = RC_WRITE_FAILED;
				printError(return_code);

				return return_code;
			}
		}

		i += 1;
	}

	return return_code;
}

//defining pinPage function to ensure that a specified page is available and pinned in the buffer pool's frames
extern RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
				  const PageNumber pageNum)
{
	// The code casts bm->mgmtData to a pointer of type Frame* and assigns it to the frame pointer.
	Frame *frame = (Frame *)bm->mgmtData;

	// if condition to handle the case where a requested page is not present in the buffer pool.
	if (frame[0].bm_PageHandle.pageNum == -1)
	{
		SM_FileHandle fh;
		openPageFile(bm->pageFile, &fh);
		frame[0].bm_PageHandle.data = (SM_PageHandle)malloc(PAGE_SIZE);

		readBlock(pageNum, &fh, frame[0].bm_PageHandle.data);
		frame[0].bm_PageHandle.pageNum = pageNum;
		// It also increments the fix count
		frame[0].fixCount += 1;

		noOfPagesRead = 0;
		hit = 0;

		frame[0].hit = hit;
		page->pageNum = pageNum;
		page->data = frame[0].bm_PageHandle.data;

		return RC_OK;
	}
	// searching for a page within the buffer pool, updating various attributes related to page access, and providing access to the page's data for further operations.
	else
	{
		bool bufferFull = true;

		for (int i = 0; i < maxBufferSize; i++)
		{
			if (frame[i].bm_PageHandle.pageNum != -1)
			{
				if (frame[i].bm_PageHandle.pageNum == pageNum)
				{
					frame[i].fixCount++;
					bufferFull = false;
					hit += 1;

					frame[i].hit = bm->strategy == RS_LRU ? hit : 1;
					page->pageNum = pageNum;
					page->data = frame[i].bm_PageHandle.data;

					clockPointer++;
					break;
				}
			}
			// this code is responsible for opening a page file, reading a page from that file, and loading it into a buffer pool frame, updating various metadata to keep track of page usage and handle page replacement strategies.
			else
			{
				SM_FileHandle fh;
				openPageFile(bm->pageFile, &fh);
				frame[i].bm_PageHandle.data = (SM_PageHandle)malloc(PAGE_SIZE);

				readBlock(pageNum, &fh, frame[i].bm_PageHandle.data);
				frame[i].bm_PageHandle.pageNum = pageNum;
				frame[i].fixCount = 1;

				noOfPagesRead += 1;
				hit += 1;

				frame[i].hit = bm->strategy == RS_LRU ? hit : 1;
				page->pageNum = pageNum;
				page->data = frame[i].bm_PageHandle.data;

				bufferFull = false;
				break;
			}
		}
		// loading a page from the file system into the buffer pool, which involves dynamic memory allocation, reading from the file, and updating attributes of the frame representing the loaded page.
		if (bufferFull == true)
		{
			Frame *newPage = (Frame *)malloc(sizeof(Frame));

			SM_FileHandle fh;
			openPageFile(bm->pageFile, &fh);
			// allocates memory to hold the data for a new page in the buffer pool
			newPage->bm_PageHandle.data = (SM_PageHandle)malloc(PAGE_SIZE);

			readBlock(pageNum, &fh, newPage->bm_PageHandle.data);

			newPage->bm_PageHandle.pageNum = pageNum;
			newPage->dirtyCount = 0;
			newPage->fixCount = 1;

			noOfPagesRead += 1;
			hit += 1;
			// handling some aspects of buffer pool management. The value of hit is determined based on the replacement strategy, and the pageNum and data fields are set in the page structure based on the values in newPage.
			newPage->hit = bm->strategy == RS_LRU ? hit : 1;
			page->pageNum = pageNum;
			page->data = newPage->bm_PageHandle.data;

			if (bm->strategy == RS_FIFO)
			{
				// implementing the logic of the FIFO page replacement strategy for managing and maintaining the buffer pool according to the FIFO strategy
				FIFO(bm, newPage, noOfPagesRead, noOfPagesWrite, maxBufferSize);
			}
			// to handle page replacement according to the LRU algorithm. In LRU (Least Recently Used), the page that has not been accessed for the longest time is the one selected for replacement.
			else if (bm->strategy == RS_LRU)
			{
				// Using LRU algorithm
				LRU(bm, newPage, maxBufferSize, noOfPagesWrite);
			}
			// The code you provided is part of an if-else block that determines the replacement strategy for the buffer manager (bm).If the strategy is RS_CLOCK, it indicates that the CLOCK algorithm for page replacement is being used.
			else if (bm->strategy == RS_CLOCK)
			{
				// Using CLOCK algorithm
				CLOCK(bm, newPage, clockPointer, maxBufferSize, noOfPagesWrite);
			}
		}
		return RC_OK;
	}
}

// Statistics Interface

//defining function to obtain information about which pages are currently loaded in the memory buffer pool.
PageNumber *getFrameContents(BM_BufferPool *const bm)
{
	RC return_code = RC_OK;
	// to obtain information about which pages are currently loaded in the memory buffer pool.
	PageNumber *frameContents = (PageNumber *)malloc(maxBufferSize * sizeof(PageNumber));
	// checks whether the variable frameContents is equal to NULL
	if (frameContents == NULL)
	{
		// sets an error code and provides an error message to indicate this failure.
		RC_message = "The memory allocation was not initialized";
		return_code = RC_BUFFER_NOT_INIT;
		printError(*RC_message);
	}
	else
	{
		// taking the management data pointer (bm->mgmtData) which was of a different data type (possibly void* or some other type) and converting it into a pointer of type Frame*
		Frame *pageFrame = (Frame *)bm->mgmtData;
		int i = 0;
		// creating a loop iterating through a set of page frames (or buffer frames) and determines the contents of each frame.
		while (i < maxBufferSize)
		{
			// building an array frameContents that represents the page numbers stored in each frame of the pageFrame array. If a frame contains a valid page, its page number is stored in frameContents.
			if ((pageFrame[i].bm_PageHandle).pageNum != -1)
			{
				frameContents[i] = (pageFrame[i].bm_PageHandle).pageNum;
			}
			// If a frame is empty or invalid, it's marked with the constant NO_PAGE
			else
			{
				frameContents[i] = NO_PAGE;
			}

			i += 1;
		}
	}

	return frameContents;
};

//retrieves the dirty status of each page in the buffer pool and returns an array of boolean values that can be used to determine which pages have been modified (are dirty) and may need to be written back to storage.
bool *getDirtyFlags(BM_BufferPool *const bm)
{
	RC return_code = RC_OK;

	// defining variable dirtyFlagCounts to keep track of whether certain items or conditions are "dirty" or "true"
	bool *dirtyFlagCounts = (bool *)malloc(maxBufferSize * sizeof(bool));

	// The code snippet you provided checks whether the dirtyFlagCounts pointer is NULL and checks for memory allocation failure, sets error codes and messages, and attempts to inform the user or developer about the problem by printing an error message.
	if (dirtyFlagCounts == NULL)
	{
		// Handle memory allocation failure
		RC_message = "The memory allocation was not initialized";
		return_code = RC_BUFFER_NOT_INIT;
		printError(*RC_message);
	}
	else
	{
		// processing the frames in the buffer pool to determine whether each frame is dirty or not and updating the dirtyFlagCounts array, which likely keeps track of the dirty status of each frame in the buffer pool.
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
	// allocating dynamic memory for an array of integers (int) and storing the address of the first element of that array in the pointer variable fixCounts.
	int *fixCounts = (int *)malloc(maxBufferSize * sizeof(int));
	// checking if the fixCounts pointer is NULL, and if so, it sets an error message and code to indicate that the buffer or memory allocation was not properly initialized.
	if (fixCounts == NULL)
	{
		// Handle memory allocation failure
		RC_message = "The memory allocation was not initialized";
		return_code = RC_BUFFER_NOT_INIT;
		printError(*RC_message);
	}
	// traversing through the frames in the buffer pool, extracting the fix count (number of clients using a page) from each frame, and storing it in the fixCounts array. If a frame is not in use, it's marked with NO_PAGE.
	else
	{
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
//retrieving the number of times pages have been read from external storage into the buffer pool
int getNumReadIO(BM_BufferPool *const bm)
{
	RC return_code = RC_OK;
	// this code checks if a variable noOfPagesRead is defined. If it's not defined (i.e., NULL), it generates an error message, sets an error code, and prints the error message.
	if (noOfPagesRead == NULL)
	{
		RC_message = "The variable is not defined";
		return_code = RC_IM_KEY_NOT_FOUND;
		printError(*RC_message);

		return return_code;
	}
	// If noOfPagesRead is defined, it increments its value by 1 and returns the result.
	return noOfPagesRead + 1;
};

//defining getNumWriteIO function to calculate how many times pages have been written from the buffer pool back to the disk or storage medium.
int getNumWriteIO(BM_BufferPool *const bm)
{
	RC return_code = RC_OK;
	// checks if noOfPagesWrite is NULL. If it is, it sets an error message and an error code and then returns that error code.
	if (noOfPagesWrite == NULL)
	{
		RC_message = "The variable is not defined";
		return_code = RC_IM_KEY_NOT_FOUND;
		printError(*RC_message);

		return return_code;
	}
	// If noOfPagesWrite is not NULL, it simply returns the value of noOfPagesWrite
	return noOfPagesWrite;
};