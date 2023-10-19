#ifndef REPLACEMENT_MGR_STRAT_H
#define REPLACEMENT_MGR_STRAT_H

#include "buffer_mgr.h"

// Replacement Strategies

extern void FIFO (BM_BufferPool *const bm, Frame *frame, int noOfPagesRead, int noOfPagesWrite, int maxBufferSize);
extern void LRU (BM_BufferPool *const bm, Frame *frame, int maxBufferSize, int noOfPagesWrite);
extern void CLOCK (BM_BufferPool *const bm, Frame *frame, int clockPointer, int maxBufferSize, int noOfPagesWrite);

#endif