# Buffer Manager for Disk-Based Storage

This C code represents a basic buffer manager for managing pages in memory, interfacing with a disk-based storage manager. The code includes functions for initializing and shutting down a buffer pool, reading and writing pages, and implementing various page replacement strategies. It also maintains statistics such as the number of pages read and written.

## Running the Script
Follow these steps to run the script:

Step 1: Build the project using `make -f Makefile`.
Step 2: Run the first test using `./test_assign2_1`.
Step 3: Run the second test using `./test_assign2_2`.

## Contributions
- Pratik Jadhav: (Implemented most of the functions of the Buffer Manager, FIFO(), forceFlushPool(), pinPage()): 20%
- Priyanka Karan: (LRU(), initBufferPool(), forcePage()): 20%
- Shikha Sharma: (LRU(), shutdownBufferPool(), PageNumber *getFrameContents()): 20%
- Vedashree Shinde: (Clock(), markDirty(), getDirtyFlags()): 20%
- Vedant Landge: (Clock(), unpinPage(), getFixCounts()): 20%

## Functions

### `RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,void *stratData)`
This function initializes a buffer pool by setting attributes such as the number of pages, page file name, and replacement strategy. It allocates memory for frames, initializes frame attributes, and assigns the frame array to the buffer pool. If any step fails, it sets an error code and returns immediately. Otherwise, it prints a success message and returns `RC_OK`.

### `RC shutdownBufferPool(BM_BufferPool *const bm)`
This function is responsible for shutting down the buffer pool. It ensures that all modified pages are written to disk and frees memory allocated for frames. It returns `RC_OK` on success or `RC_PINNED_PAGES_ERROR` if some pages are still in use.

### `RC forceFlushPool(BM_BufferPool *const bm)`
Used in the `shutdownBufferPool` function to force writing all dirty pages to disk.

### `RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)`
Marks a page as dirty, indicating that it needs to be written to disk when necessary.

### `RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)`
Decrements the fixCount of a specific page in the buffer pool by 1, indicating that the page is no longer in use.

### `RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)`
Forces (writes) a specific page from the buffer pool to the associated page file on disk.

### `RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)`
Pins a specific page in the buffer pool. If the buffer pool is empty or the requested page is not in memory, it reads the page from the associated page file, manages page replacement strategies for a full buffer, and updates relevant counters.

### `PageNumber *getFrameContents(BM_BufferPool *const bm)`
Retrieves an array of page numbers that represent the contents of frames in the buffer pool.

### `bool *getDirtyFlags(BM_BufferPool *const bm)`
Retrieves an array of boolean flags that indicate whether each frame in the buffer pool contains a dirty (modified) page.

### `int *getFixCounts(BM_BufferPool *const bm)`
Retrieves an array of integers representing the fix counts for each frame in the buffer pool.

### `int getNumReadIO(BM_BufferPool *const bm)`
Retrieves the number of pages that have been read from the page file into the buffer pool as part of input/output (I/O) operations.

### `int getNumWriteIO(BM_BufferPool *const bm)`
Retrieves the number of pages that have been written from the buffer pool to the page file as part of output/input (I/O) operations.

## Replacement Strategies
The buffer manager supports the following replacement strategies:

### FIFO (First-In-First-Out)
Implements the FIFO page replacement strategy for managing pages in the buffer pool. Ensures that the least recently used page is replaced when the buffer is full.

### LRU (Least Recently Used)
Simulates page replacement using the LRU strategy, replacing the least recently used page.

### CLOCK
Simulates page replacement using the CLOCK algorithm, a variant of the second chance or clock page replacement strategy.

Feel free to explore the code and use the provided functions for managing buffer pools and page replacements in your projects.

