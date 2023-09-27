#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "buffer_mgr.h"
#include "replacement_mgr_strat.h"
#include "storage_mgr.h"


void FIFO(BM_BufferPool *const bm, Frame *page, int rearIndex, int maxBufferSize, int noOfPagesWrite)
{
	Frame *frame = (Frame *) bm->mgmtData;
	
	int frontIndex = rearIndex % maxBufferSize;

	for(int i = 0; i < maxBufferSize; i++)
	{
		if(frame[frontIndex].fixCount == 0)
		{
			if(frame[frontIndex].dirtyCount == 1)
			{
				SM_FileHandle *fHandle;
				openPageFile(bm->pageFile, fHandle);
				writeBlock((frame[frontIndex].bm_PageHandle).pageNum, fHandle, frame[frontIndex].smp);
				
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

int findClockPage(Frame* frames, int numFrames, int* clockHand) {
    while (1) {
        if (!frames[*clockHand].referenceBit) {
            // This page has a reference bit of false and can be replaced
            return *clockHand;
        } else {
            // Give the page a "second chance" by resetting the reference bit
            frames[*clockHand].referenceBit = false;
            *clockHand = (*clockHand + 1) % numFrames;
        }
    }
}

// Function to simulate page replacement using CLOCK
void CLOCK(Frame* frames, int numFrames, int pageNum, int* clockHand) {
    int emptySlot = -1;

    // Check if the page is already in memory
    for (int i = 0; i < numFrames; i++) {
        if (frames[i].bm_PageHandle.pageNum == pageNum) {
            // Page is already in memory, so just update the fix count and set the reference bit
            frames[i].fixCount++;
            frames[i].referenceBit = true;
            return;
        }
        if (frames[i].fixCount == 0 && emptySlot == -1) {
            // Found an empty slot to place a new page (fix count is zero)
            emptySlot = i;
        }
    }

    // If no empty slot is found, evict a page using the CLOCK algorithm
    if (emptySlot == -1) {
        emptySlot = findClockPage(frames, numFrames, clockHand);
    }

    // Replace the selected page with the new page
    frames[emptySlot].bm_PageHandle.pageNum = pageNum;
    frames[emptySlot].fixCount = 1;
    frames[emptySlot].dirtyCount = 0;
    frames[emptySlot].referenceBit = true; // Set the reference bit to true for the new page
}

int findLRUPage(Frame* frames, int numPages) {
    int minIndex = 0;
    int minLRU = frames[0].fixCount;

    for (int i = 1; i < numPages; i++) {
        if (frames[i].fixCount < minLRU) {
            minLRU = frames[i].fixCount;
            minIndex = i;
        }
    }

    return minIndex;
}

// Function to simulate page replacement using LRU
void LRU(Frame* frames, int numPages, int pageNumber) {
    int emptySlot = -1;

    // Check if the page is already in memory
    for (int i = 0; i < numPages; i++) {
        if (frames[i].bm_PageHandle.pageNum == pageNumber) {
            // Page is already in memory, so just update the fix count
            frames[i].fixCount++;
            return;
        }
        if (frames[i].fixCount == 0 && emptySlot == -1) {
            // Found an empty slot to place a new page (fix count is zero)
            emptySlot = i;
        }
    }

    // If no empty slot is found, evict the LRU page with fix count zero
    if (emptySlot == -1) {
        emptySlot = findLRUPage(frames, numPages);
    }

    // Replace the LRU page with the new page
    frames[emptySlot].bm_PageHandle.pageNum = pageNumber;
    frames[emptySlot].fixCount = 1;
}

void LFU (BM_PageHandle *const page){

};

void LRU_k (BM_PageHandle *const page){

};