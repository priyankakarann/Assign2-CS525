#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "buffer_mgr.h"
#include "replacement_mgr_strat.h"
#include "storage_mgr.h"


extern void FIFO(BM_BufferPool *const bm, Frame *frame, int noOfPagesRead, int noOfPagesWrite, int maxBufferSize)
{
	Frame *pageFrame = (Frame *) bm->mgmtData;
	
	int frontIndex = noOfPagesRead % maxBufferSize;

	for(int i = 0; i < maxBufferSize; i++)
	{
		if(pageFrame[frontIndex].fixCount == 0)
		{
			if(pageFrame[frontIndex].dirtyCount == 1)
			{
				SM_FileHandle fh;
				openPageFile(bm->pageFile, &fh);
				writeBlock(pageFrame[frontIndex].bm_PageHandle.pageNum, &fh, pageFrame[frontIndex].bm_PageHandle.data);
				
				noOfPagesWrite++;
			}
			
			pageFrame[frontIndex].bm_PageHandle.data = frame->bm_PageHandle.data;
			pageFrame[frontIndex].bm_PageHandle.pageNum = frame->bm_PageHandle.pageNum;
			pageFrame[frontIndex].dirtyCount = frame->dirtyCount;
			pageFrame[frontIndex].fixCount = frame->fixCount;
			break;
		}
		else
		{
			frontIndex++;
			frontIndex = (frontIndex % maxBufferSize == 0) ? 0 : frontIndex;
		}
	}
}

extern void LRU(BM_BufferPool *const bm, Frame *frame, int maxBufferSize, int noOfPagesWrite)
{	
	Frame *pageFrame = (Frame *) bm->mgmtData;
	int i, leastHitIndex, leastHit;

	for(i = 0; i < maxBufferSize; i++)
	{
		if(pageFrame[i].fixCount == 0)
		{
			leastHitIndex = i;
			leastHit = pageFrame[i].hit;
			break;
		}
	}	

	for(i = leastHitIndex + 1; i < maxBufferSize; i++)
	{
		if(pageFrame[i].hit < leastHit)
		{
			leastHitIndex = i;
			leastHit = pageFrame[i].hit;
		}
	}

	if(pageFrame[leastHitIndex].dirtyCount == 1)
	{
		SM_FileHandle fh;
		openPageFile(bm->pageFile, &fh);
		writeBlock(pageFrame[leastHitIndex].bm_PageHandle.pageNum, &fh, pageFrame[leastHitIndex].bm_PageHandle.data);
		
		noOfPagesWrite++;
	}
	
	pageFrame[leastHitIndex].bm_PageHandle.data = frame->bm_PageHandle.data;
	pageFrame[leastHitIndex].bm_PageHandle.pageNum = frame->bm_PageHandle.pageNum;
	pageFrame[leastHitIndex].dirtyCount = frame->dirtyCount;
	pageFrame[leastHitIndex].fixCount = frame->fixCount;
	pageFrame[leastHitIndex].hit = frame->hit;
}


extern void CLOCK(BM_BufferPool *const bm, Frame *frame, int clockPointer, int maxBufferSize, int noOfPagesWrite)
{	
	Frame *pageFrame = (Frame *) bm->mgmtData;
	while(true)
	{
		clockPointer = (clockPointer % maxBufferSize == 0) ? 0 : clockPointer;

		if(pageFrame[clockPointer].hit == 0)
		{
			if(pageFrame[clockPointer].dirtyCount == 1)
			{
				SM_FileHandle fh;
				openPageFile(bm->pageFile, &fh);
				writeBlock(pageFrame[clockPointer].bm_PageHandle.pageNum, &fh, pageFrame[clockPointer].bm_PageHandle.data);
				
				noOfPagesWrite++;
			}
			
			pageFrame[clockPointer].bm_PageHandle.data = frame->bm_PageHandle.data;
			pageFrame[clockPointer].bm_PageHandle.pageNum = frame->bm_PageHandle.pageNum;
			pageFrame[clockPointer].dirtyCount = frame->dirtyCount;
			pageFrame[clockPointer].fixCount = frame->fixCount;
			pageFrame[clockPointer].hit = frame->hit;
			clockPointer++;
			break;	
		}
		else
		{
			pageFrame[clockPointer++].hit = 0;		
		}
	}
}