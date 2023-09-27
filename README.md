**Running the script**
Step 1-  $ make -f Makefile
Step 2-  $ ./test_assign2_1
Step 3-  $ ./test_assign2_2

**Contributions**
Priyanka Karan: (LRU(), initBufferPool(), forcePage()): 20%
Shikha Sharma: (LRU(), shutdownBufferPool(), PageNumber *getFrameContents()): 20%
Pratik Jadhav: (Implemented most of the functions of the Buffer Manager, FIFO(), forceFlushPool(), pinPage()): 20%
Vedashree Shinde: (Clock(), markDirty(), getDirtyFlags()): 20%
Vedant Landge: (Clock(), unpinPage(), getFixCounts()): 20%

---Introduction---

This C code represents a basic buffer manager for managing pages in memory, interfacing with a disk-based storage manager. The code includes functions for initializing and shutting down a buffer pool, reading and writing pages, and implementing a FIFO page replacement strategy. It also maintains statistics such as the number of pages read and written.

**RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,void *stratData)**

This function initializes a buffer pool by setting attributes such as the number of pages, page file name, and replacement strategy. It allocates memory for frames, initializes frame attributes, and assigns the frame array to the buffer pool. If any step fails, it sets an error code and returns immediately. Otherwise, it prints a success message and returns `RC_OK`.


**shutdownBufferPool(BM_BufferPool *const bm)**
This function is responsible for shutting down the buffer pool.
Initialize return_code to RC_OK.
Retrieve the Frame array (frame) from the `bm->mgmtData` field.
Call `forceFlushPool(bm)` to flush all dirty pages to disk before shutting down. This ensures that any modified pages are written to disk.
Iterate through the frames in the buffer pool. If any frame has a non-zero fixCount (indicating it's pinned), it returns RC_PINNED_PAGES_ERROR, indicating that some pages are still in use and cannot be evicted.
If all frames have a fixCount of 0, it frees the memory allocated for the frames (frame) and returns RC_OK.

**fRC forceFlushPool(BM_BufferPool *const bm):**:

Used in the above shutdownbufferpool function
Initialize return_code to RC_OK.
Retrieve the Frame array (frame) from the bm->mgmtData field.
Iterate through the frames in the buffer pool. For each frame:
If the `fixCount` is 0 and the `dirtyCount` is 1 , it proceeds to write the page to disk.
It opens the page file associated with the buffer pool using openPageFile.
Writes the content of the frame to the appropriate block in the file using writeBlock.
Resets the dirtyCount to 0 to indicate that the page is now clean.
Increments noOfPagesWrite, which seems to be a global counter tracking the number of pages written to disk.

**RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)**:

Initialize return_code to RC_OK.
Retrieve the Frame array (frame) from the bm->mgmtData field.
Iterate through the frames in the buffer pool. For each frame:
Check if the pageNum of the page in the frame matches the pageNum of the page passed as a parameter (page).
If there's a match, set the dirtyCount of that frame to 1, indicating that the page is now dirty and needs to be written to disk when necessary.

**RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page):**

This function decrements the fixCount of a specific page in the buffer pool by 1, indicating that the page is no longer in use (unpinned). It searches for the page in the frames of the buffer pool, updates the fixCount, and returns RC_OK to indicate a successful operation.

**RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page):**

This function forces (writes) a specific page from the buffer pool to the associated page file on disk. It searches for the page in the buffer pool frames, opens the page file, and writes the page's content to the file using writeBlock. If the write operation fails, it returns an error code RC_WRITE_FAILED; otherwise, it returns RC_OK to indicate success.

**RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)**

This function is responsible for pinning a specific page in the buffer pool. If the buffer pool is empty or the requested page is not in memory, it reads the page from the associated page file, manages page replacement strategies for a full buffer, and updates relevant counters. It returns RC_OK on successful pinning or specific error codes for encountered issues.

**PageNumber *getFrameContents(BM_BufferPool *const bm):**

This function retrieves an array of page numbers that represent the contents of frames in the buffer pool. It first allocates memory for the array and handles memory allocation failure. Then, it iterates through the frames, populating the array with the page numbers currently held in the frames. If a frame is empty (not holding any page), it assigns NO_PAGE to that position in the array. Finally, it returns the array of page numbers or an error code if memory allocation fails.

**bool *getDirtyFlags(BM_BufferPool *const bm):**

This function retrieves an array of boolean flags that indicate whether each frame in the buffer pool contains a dirty (modified) page. It allocates memory for the array and handles memory allocation failure. Then, it iterates through the frames, checking the dirtyCount attribute of each frame. If dirtyCount is 1, it sets the corresponding flag to true, indicating that the page in that frame is dirty; otherwise, it sets the flag to false. Finally, it returns the array of boolean flags or an error code if memory allocation fails. This function helps identify which pages need to be written back to disk.

**int *getFixCounts(BM_BufferPool *const bm)**

This function retrieves an array of integers representing the fix counts for each frame in the buffer pool. It allocates memory for the array and handles memory allocation failure. Then, it iterates through the frames, checking the fixCount attribute of each frame. If fixCount is not -1, it assigns the value of fixCount to the corresponding position in the array, indicating how many clients have pinned that page. If fixCount is -1, it assigns NO_PAGE to indicate an empty frame. Finally, it returns the array of fix counts or an error code if memory allocation fails. This function helps track the usage of pages in the buffer pool.

**int getNumReadIO(BM_BufferPool *const bm)**

This function retrieves the number of pages that have been read from the page file into the buffer pool as part of input/output (I/O) operations. It first checks if the variable noOfPagesRead is defined, and if not, it returns an error code RC_IM_KEY_NOT_FOUND, indicating that the variable is not available. If the variable is defined, it returns the count of read I/O operations stored in noOfPagesRead. This function helps track the efficiency and performance of read operations in the buffer pool.

**int getNumWriteIO(BM_BufferPool *const bm)**

This function retrieves the number of pages that have been written from the buffer pool to the page file as part of output/input (I/O) operations. It first checks if the variable noOfPagesWrite is defined, and if not, it returns an error code RC_IM_KEY_NOT_FOUND, indicating that the variable is not available. If the variable is defined, it returns the count of write I/O operations stored in noOfPagesWrite. This function helps track the efficiency and performance of write operations in the buffer pool, providing insight into how many times pages were written back to the storage medium.

---------------------------Replacement strategies-------------------------------

**void FIFO(BM_BufferPool *const bm, Frame *page):**

This function implements the First-In-First-Out (FIFO) page replacement strategy for managing pages in the buffer pool. It takes a newly allocated page (page) and replaces an existing page in the buffer pool based on the FIFO policy. It iterates through the frames, looking for the first frame with a fix count of 0 (indicating it's not currently in use). If it finds such a frame, it checks if it needs to write the page back to disk (if it's dirty), updates the frame with the new page's content, and breaks out of the loop. If all frames are currently in use, it wraps around to the beginning of the frame array to find an available frame. This function helps ensure that the least recently used page is replaced when the buffer is full, maintaining a simple queue-like behavior for page replacement.

**void LRU(BM_BufferPool *const bm, Frame *page):**
Simulates page replacement using the LRU (Least Recently Used) strategy. It defines a structure called Page to represent pages in memory, with each page having a page number and a fix count. The findLRUPage function identifies the least recently used page based on their fix counts. The replacePage function handles page replacements, updating fix counts and evicting the LRU page if necessary. The main function initializes a memory buffer, simulates page requests, and prints the pages currently in memory after all requests, demonstrating how LRU works for managing page replacements in a limited memory environment.

**void CLOCK(BM_BufferPool *const bm, Frame *page):**
Simulates page replacement using the CLOCK algorithm, which is a variant of the second chance or clock page replacement strategy. It defines a structure called Frame to represent page frames in memory, including page number, fix count, dirty count, and a reference bit for the CLOCK algorithm. The replacePageClock function handles page replacements, updating fix counts, and using the reference bit to determine which page to replace. The main function initializes a memory buffer, simulates page requests, and prints the frames currently in memory after all requests, demonstrating how the CLOCK algorithm manages page replacements in a limited memory environment, giving pages a "second chance" before eviction.
