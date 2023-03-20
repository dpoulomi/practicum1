The output file is heap_output. 

The requestMemoryPageInMainMemory(pageId) which internally calls the requestMemoryPageInDisk(pageId),pm_malloc_to_disk(intptr_t pageId, char byteArray[4]) and  getLeastRecentlyUsedPageId() methods as and when necessary.
The  pm_malloc(data, size) function takes in two parameters data and the size of the data in the form of void* arguments to support multithreading.
The pm_free(index) function takes the index of the block which needs to be freed in the form of void* arguments to support multithreading..

Each block in the memory has the following fields:
    Address: The index location of the first byte on the heap covered by the current block. Remember, the heap is simply a char array.
    Size: The size of the current memory block. In other words, how many bytes does this block cover.
    Next: A pointer to the next block, should a piece of data be split between multiple blocks. This can be NULL.
    isFree: A flag variable that marks whether or not this memory block is free to be used, or reserved for something.


Scenarios tested are as belows:
   1)Several threads try to allocate memory with different sets of data. Test shows the data gets added sequencially. 
   2)Multiple threads try to free the memory and there is no overlapping.
   3)Try to allocate and free  and then again allocate and free, which happens in a definite sequence as well with no data corruption in heap.
   4)Trying to allocate, retrieve, dump, then again allocate, dump retrieve and free in different orders to show all possible scenarios 
   happening with no overlap.

Assumptions:

1)I am assuming that at a time only 4 bytes of data can be allocated as a page size in my implementation is of size 4.
2)If the size exceeds the data will be saved to a disk and an id will be provided to the user irrespective of whether it is saved to 
main memory or disk. The user can retrieve the data using that id.
3)In order to demonstrate the main memory is exhausted and to be able to save and retrieve data from secondary memory have considered the memory block to be a very small size of 40 bytes and each page being of equal size of 4 bytes, the total execution will allow a total of 10 memory pages.







