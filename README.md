The output file is heap_output. 


The  pm_malloc(data, size) function takes in two parameters data and the size of the data.
The pm_free(index) function takes the index of the block which needs to be freed.

Each block in the memory has the following fields:
    Address: The index location of the first byte on the heap covered by the current block. Remember, the heap is simply a char array.
    Size: The size of the current memory block. In other words, how many bytes does this block cover.
    Next: A pointer to the next block, should a piece of data be split between multiple blocks. This can be NULL.
    isFree: A flag variable that marks whether or not this memory block is free to be used, or reserved for something.


Scenarios tested are as belows:
   1)Several threads try to allocate memory with different sets of data. Test shows the data gets added sequencially. 
   2)Multiple threads try to free the memory and there is no overlapping.
   3)Try to allocate and free  and then again allocate and free, which happens in a definite sequence as well with no data corruption in heap.







