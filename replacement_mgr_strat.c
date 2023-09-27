// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>

// #include "buffer_mgr.h"
// #include "replacement_mgr_strat.h"
// #include "storage_mgr.h"


// void FIFO(BM_BufferPool *const bm, Frame *page, int rearIndex, int maxBufferSize, int noOfPagesWrite)
// {
// 	Frame *frame = (Frame *) bm->mgmtData;
	
// 	int frontIndex = rearIndex % maxBufferSize;

// 	for(int i = 0; i < maxBufferSize; i++)
// 	{
// 		if(frame[frontIndex].fixCount == 0)
// 		{
// 			if(frame[frontIndex].dirtyCount == 1)
// 			{
// 				SM_FileHandle *fHandle;
// 				openPageFile(bm->pageFile, fHandle);
// 				writeBlock((frame[frontIndex].bm_PageHandle).pageNum, fHandle, frame[frontIndex].smp);
				
// 				noOfPagesWrite++;
// 			}
			
// 			frame[frontIndex].smp = page->smp;
// 			(frame[frontIndex].bm_PageHandle).pageNum = (page->bm_PageHandle).pageNum;
// 			frame[frontIndex].dirtyCount = page->dirtyCount;
// 			frame[frontIndex].fixCount = page->fixCount;
// 			break;
// 		}
// 		else
// 		{
// 			frontIndex += 1;
//             if (frontIndex % maxBufferSize == 0){
//                 frontIndex = 0;
//             }
// 		}
// 	}
// }


// void LRU (BM_PageHandle *const page){

// };

// void CLOCK (BM_BufferPool *const bm){

// };

// void LFU (BM_PageHandle *const page){

// };

// void LRU_k (BM_PageHandle *const page){

// };