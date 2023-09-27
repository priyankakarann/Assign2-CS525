#ifndef REPLACEMENT_MGR_STRAT_H
#define REPLACEMENT_MGR_STRAT_H

#include "buffer_mgr.h"

// Replacement Strategies

extern void FIFO (BM_BufferPool *const bm, Frame *page, int rearIndex, int maxBufferSize, int noOfPagesWrite);
extern void LRU (Frame* frames, int numPages, int pageNumber);
extern void CLOCK (Frame* frames, int numFrames, int pageNum, int* clockHand);
extern void LFU (BM_PageHandle *const page);
extern void LRU_k (BM_PageHandle *const page);

#endif
