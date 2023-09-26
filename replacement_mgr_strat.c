#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "buffer_mgr.h"
#include "replacement_mgr_strat.h"
#include "storage_mgr.h"


void FIFO(BM_BufferPool *const bm, Frame *page, int rearIndex, int maxBufferSize, int noOfPagesWrite)
{
	Frame *pageFrame = (Frame *) bm->mgmtData;
	
	int i, frontIndex;
	frontIndex = rearIndex % maxBufferSize;

	for(i = 0; i < maxBufferSize; i++)
	{
		if(pageFrame[frontIndex].fixCount == 0)
		{
			if(pageFrame[frontIndex].dirtyCount == 1)
			{
				SM_FileHandle *fHandle;
				openPageFile(bm->pageFile, fHandle);
				writeBlock(pageFrame[frontIndex].bm_PageHandle.pageNum, fHandle, pageFrame[frontIndex].smp);
				
				noOfPagesWrite++;
			}
			
			pageFrame[frontIndex].smp = page->smp;
			pageFrame[frontIndex].bm_PageHandle.pageNum = page->bm_PageHandle.pageNum;
			pageFrame[frontIndex].dirtyCount = page->dirtyCount;
			pageFrame[frontIndex].fixCount = page->fixCount;
			break;
		}
		else
		{
			frontIndex++;
            if (frontIndex % maxBufferSize == 0){
                frontIndex = 0;
            }
		}
	}
}


void LRU (BM_PageHandle *const page){

};

void CLOCK (BM_BufferPool *const bm){

};

void LFU (BM_PageHandle *const page){

};

void LRU_k (BM_PageHandle *const page){

};