#include "pm_heap.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

// #define HEAP_SIZE 10 * 1024 * 1024
// #define MAX_BLOCK 2621440
#define HEAP_SIZE 10 * 4
#define MAX_BLOCK 10
#define page_size 4
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
time_t times[MAX_BLOCK];
char *requestMemoryPageInDisk(int pageId);
intptr_t getLeastRecentlyUsedPageId();
void pm_malloc_to_disk(intptr_t pageId, char byteArray[]);
void updateTimeArray(int address, block_t *currBlock);
int diskPageId = 0;

struct MemoryBlock
{
	char isFree;
	intptr_t address;
	int size;
	block_t *next;
};

struct arg_struct_malloc
{
	char *values;
	int size;
};

struct arg_struct_free_dump
{
	int size;
};

char heap[HEAP_SIZE] = {0};
block_t allocMemBlocks[MAX_BLOCK];

int blockSize = 0;

char* requestMemoryPageInMainMemory(intptr_t pageId)
{

	// pthread_mutex_lock(&lock);
	// struct arg_struct_free_dump *args = arguments;
	// printf("test");
	block_t *currBlock = NULL;
	char searchedData[6];
	char* retValue;
	// char *searchedData ;

	intptr_t address = pageId;
	int j = 0;
	if (address < HEAP_SIZE)
	{
		for (int i = 0; i < blockSize; i++)
		{
			currBlock = &(allocMemBlocks[i]);

			if (currBlock->address == address && currBlock->isFree == 0)
			{
				// 	// return *currBlock;
				printf("Searched data is found in main memory at pageId %d \n", currBlock->address);
				for (int i = pageId; i < (pageId + 4); i++)
				{
					// *(searchedData+j) = heap[i];
					searchedData[j] = heap[i];
					j++;
				}
				// *(searchedData+j) = '\0';
				// printf("%d", j);
				searchedData[j] = '\0';
				updateTimeArray(pageId, currBlock);
				// for(int i = 0; i < 5; i++)
				// 	{
				// 	printf("%c", searchedData[i]);
				// 	}
				// pthread_mutex_unlock(&lock);
				// printf("Inside : %s \n", searchedData);
				return searchedData;
			}
		}
	}
	

	else
	{
		printf("when data not not found in memory");
		//strcpy(searchedData, requestMemoryPageInDisk(pageId));
		char* retValue = requestMemoryPageInDisk(pageId);

		// // check fr page fault
		// // if (searchedData[0] != NULL)
		// if (sizeof(searchedData) == 5)
		// {
		// 	printf("when data not not found in memory and disk and page fault happens");
		// 	intptr_t pageIdToReplace = getLeastRecentlyUsedPageId();
		// 	char data[4] = {0};
		// 	strcpy(data, heap[pageIdToReplace]);

		// 	pm_malloc_to_disk(pageIdToReplace, data);
		// 	pm_free(pageIdToReplace);
		// 	struct arg_struct_malloc mallocdata;
		// 	mallocdata.values = searchedData;
		// 	mallocdata.size = 4;
		// 	pm_malloc(&mallocdata);
		// }
		// else
		// {
		// 	// page fault code
		// 	intptr_t pageIdToReplace = getLeastRecentlyUsedPageId();
		// 	pm_free(pageIdToReplace);
		// 	struct arg_struct_malloc mallocdata;
		// 	mallocdata.values = searchedData;
		// 	mallocdata.size = 4;
		// 	pm_malloc(&mallocdata);
		// }
	}

	// add the least recently used page to disk and the searched page to
	// back to main memory in place of the recently used memory.

	//updateTimeArray(address, currBlock);
	// pthread_mutex_unlock(&lock);
	return retValue;
}
intptr_t getLeastRecentlyUsedPageId()
{
	intptr_t leastRecentlyUsedPage = times[0];
	for (int i = 1; i < sizeof(times); i++)
	{
		if (times[i] < leastRecentlyUsedPage)
		{
			leastRecentlyUsedPage = times[i];
		}
	}
	return leastRecentlyUsedPage;
}

FILE *f;
void pm_malloc_to_disk(intptr_t pageId, char byteArray[4])
{

	f = fopen("backing_store.csv", "a");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	/* print some text */
	// fprintf(f, "PageId, Data\n");
	for (int i = 0; i < 4; i++)
	{
		printf("%c", byteArray[i]);
	}

	// printf("Starting pm_malloc! %d \n", blockSize);
	printf("\n");
	fprintf(f, "%ld, %s\n", pageId, byteArray);
	printf("Data saved to csv file \n");
	fclose(f);
}

char *requestMemoryPageInDisk(int pageId)
{
	printf("read the csv \n");

	f = fopen("backing_store.csv", "r");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	char buffer[80];
	int row = 0;
    int column = 0;
	char searchedData[5];
	bool found = false;
	while (fgets(buffer, 80, f))
	{
		printf("read the inside the csv \n");
		column = 0;
		// If you only need the first column of each row
		// char *token = strtok(buffer, ", ");
		// printf("%s\n", token);
		//*(searchedData+j)
		// if ((atoi(*(token + 0))) == pageId) {
		//     // int n = atoi(token);
		// 	searchedData = &token[1];
		//     printf("Data is %s\n", searchedData);
		// }

		// If you need all the values in a row
		char *token = strtok(buffer, ", ");
		while (token)
		{
			printf("read all the values in a row\n");
			// Just printing each integer here but handle as needed
			// int n = atoi(token);
			// printf("%d\n", n);
			if (column == 0 && atoi(token) == pageId)
			{
				found = true;
			}
			if (column == 1 && found == true)
			{
				// for(int i = 0 ; i < 4; i++)
				// {
				// 	searchedData[i] = token[i];
				// }
				// searchedData[4] = '\0';
				// printf("Found data in disk is: %s\n", searchedData);
				printf("Found data in disk is: %s\n", token);
				return token;
			}
			// printf("%s\n", token);
			token = strtok(NULL, ",");
			column++;
		}

		// char *value = strtok(buffer, ", ");
		// while (value)
		// {
		// 	// Column 1
		// 	if (column == 0 && (atoi(*(value + 0))) == pageId)
		// 	{
		// 		printf("Name :");
		// 	}

		// 	// Column 2
		// 	if (column == 1)
		// 	{
		// 		printf("\tAccount No. :");
		// 	}

		// 	// Column 3
		// 	if (column == 2)
		// 	{
		// 		printf("\tAmount :");
		// 	}

		// 	printf("%s", value);
		// 	value = strtok(NULL, ", ");
		// 	column++;
		// }
		
	}
	fclose(f);
	return searchedData;
}

void updateTimeArray(int address, block_t *currBlock)
{
	time_t seconds;
	times[currBlock->address] = seconds;
}

intptr_t get_max_addr()
{
	assert(blockSize < MAX_BLOCK);

	intptr_t maxAddr = 0;

	for (int i = 0; i < blockSize; i++)
	{
		// Check if memory block and its width larger than previous
		if (allocMemBlocks[i].address + allocMemBlocks[i].size > maxAddr)
		{
			// Set max address to next available address
			maxAddr = allocMemBlocks[i].address + allocMemBlocks[i].size;
		}
	}

	return maxAddr;
}

int get_block_size(intptr_t addr)
{
	int size = 0;

	for (int i = 0; i < blockSize; i++)
	{
		block_t *currBlock = &(allocMemBlocks[i]);

		if (currBlock->address == addr)
		{
			// Traverse the memory block chain
			do
			{
				size += currBlock->size;
				currBlock = currBlock->next;
			} while (currBlock != NULL);

			return size;
		}
	}

	return 0;
}

intptr_t pm_malloc(void *arguments)
{
	pthread_mutex_lock(&lock);
	if (blockSize < MAX_BLOCK)
	{

		// assert(blockSize < MAX_BLOCK);

		struct arg_struct_malloc *args = arguments;
		int size = args->size;
		char byteArray[size];
		printf("Contents of the array are: ");
		memcpy(byteArray, args->values, size);
		for (int i = 0; i < size; i++)
		{
			printf("%c", byteArray[i]);
		}

		// printf("Starting pm_malloc! %d \n", blockSize);
		printf("\n");

		// fflush(stdout);
		block_t *startBlock = NULL;
		block_t *prevBlock = NULL;
		int arrayPtr = 0;
		int hasStarted = 0;

		// Iterate through all existing memory blocks
		for (int i = 0; i < blockSize; i++)
		{
			block_t *currBlock = &(allocMemBlocks[i]);
			// printf("Starting for loop!\n");
			// fflush(stdout);
			// If current mem block is free
			if (currBlock->isFree)
			{
				// If current memory block has smaller size
				if (currBlock->size < size)
				{
					// printf("inside currBlock->size < size \n");
					// fflush(stdout);
					currBlock->isFree = 0; // Set current block to not free
					memcpy(heap + currBlock->address, byteArray + arrayPtr, currBlock->size);
					size -= currBlock->size;	 // Change left-over byte size
					arrayPtr += currBlock->size; // Increment array position

					// Check if first block in loop
					if (!hasStarted)
					{
						startBlock = currBlock;
						hasStarted = 1;
					}
					else
					{
						prevBlock->next = currBlock; // Link previous block to current block
					}

					prevBlock = currBlock; // Set prev block to current block
				}
				// If current memory block has bigger size
				else if (currBlock->size > size)
				{
					currBlock->isFree = 0; // Set current block to not free

					// Check if start block is null
					if (startBlock == NULL)
					{
						startBlock = currBlock;
					}
					// Check if there is a previous block
					else if (prevBlock != NULL)
					{
						prevBlock->next = currBlock; // Link prev block to current block
					}
					memcpy(heap + currBlock->address, byteArray + arrayPtr, size);

					// "Split" original block into a new block
					block_t newBlock;
					newBlock.isFree = 1;						  // Set new block to free
					newBlock.address = currBlock->address + size; // Address set to first byte of leftover space
					newBlock.size = currBlock->size - size;		  // Size set to leftover space size
					newBlock.next = NULL;
					allocMemBlocks[blockSize++] = newBlock; // Append new block to all mem blocks

					currBlock->size = size; // Reduce size
					currBlock->next = NULL; // Set next to NULL
					pthread_mutex_unlock(&lock);
					// printf("Write complete! in pm malloc");
					// fflush(stdout);
					return startBlock->address;
				}
				// If current memory block is equal in size
				else
				{
					// printf("inside else \n");
					// fflush(stdout);
					currBlock->isFree = 0; // Set current block to not free
					memcpy(heap + currBlock->address, byteArray, size);
					currBlock->next = NULL; // Nullify chain
					time_t seconds;
					times[currBlock->address] = seconds;
					pthread_mutex_unlock(&lock);
					return currBlock->address; // Return address
				}
				//  pthread_mutex_unlock(&lock);
			}
		}
		// printf("outside for loop! %c , %d\n", (char *)byteArray[1], size);
		// fflush(stdout);
		block_t newBlock;
		newBlock.isFree = 0; // Set block to not free
		newBlock.address = get_max_addr();
		newBlock.size = size;
		newBlock.next = NULL;
		allocMemBlocks[blockSize++] = newBlock;
		memcpy(heap + newBlock.address, byteArray + arrayPtr, size);
		time_t seconds;
		times[newBlock.address] = seconds;
		// memcpy(heap, byteArray + arrayPtr, size);

		// Check if start block is null
		if (startBlock == NULL)
		{
			startBlock = &(allocMemBlocks[blockSize - 1]);
		}
		// Check if there is a previous block
		else if (prevBlock != NULL)
		{
			prevBlock->next = &(allocMemBlocks[blockSize - 1]); // Link prev block to current block
		}

		pthread_mutex_unlock(&lock);
		return startBlock->address;
	}
	else
	{
		struct arg_struct_malloc *args = arguments;
		int size = args->size;
		char byteArray[size];
		printf("Contents of the array to be saved in disk are: ");
		memcpy(byteArray, args->values, size);
		for (int i = 0; i < size; i++)
		{
			printf("%c", byteArray[i]);
		}

		printf("\n");
		int index = MAX_BLOCK * 4 + diskPageId;
		pm_malloc_to_disk(index, byteArray);
		diskPageId = diskPageId + 4;

		pthread_mutex_unlock(&lock);
		return index;
	}
	pthread_mutex_unlock(&lock);
	return -1;
}

void pm_free(void *arguments)
{
	pthread_mutex_lock(&lock);
	struct arg_struct_free_dump *args = arguments;
	intptr_t address = args->size;
	assert(blockSize < MAX_BLOCK);

	for (int i = 0; i < blockSize; i++)
	{
		block_t *currBlock = &(allocMemBlocks[i]);

		if (currBlock->address == address && currBlock->isFree == 0)
		{
			while (currBlock != NULL)
			{
				currBlock->isFree = 1;								   // Set memory block to free
				memset(heap + currBlock->address, 0, currBlock->size); // Nullify the bytes
				currBlock = currBlock->next;						   // Set current to next mem block in chain
			}
			pthread_mutex_unlock(&lock);
			return;
		}
	}
	pthread_mutex_unlock(&lock);

	return;
}

void dump_heap(void *arguments)
{
	/*
	Just prints out the heap nothing special
	*/
	pthread_mutex_lock(&lock);
	struct arg_struct_free_dump *args = arguments;
	int size = args->size;
	// printf("inside dump_heap , %d\n", size);
	fflush(stdout);

	for (int i = 0; i < size; i++)
	{
		// printf("inside foor loop in dump_heap");
		printf(" %c |", heap[i]);
		// fflush(stdout);
	}
	printf("\n");
	// fflush(stdout);
	pthread_mutex_unlock(&lock);
}

int read_mem(intptr_t dst, char buffer[], int size)
{
	assert(get_block_size(dst) != 0);
	assert(size >= get_block_size(dst));

	int posPtr = 0;

	for (int i = 0; i < blockSize; i++)
	{
		block_t *currBlock = &(allocMemBlocks[i]);

		if (currBlock->address == dst)
		{
			// Traverse the memory block chain
			do
			{
				memcpy(buffer + posPtr, heap + currBlock->address, currBlock->size); // Copies memory from heap to buffer
				posPtr += currBlock->size;
				currBlock = currBlock->next;
			} while (currBlock != NULL);

			return 1;
		}
	}

	return 0;
}

int write_mem(intptr_t dst, char buffer[], int srcSize)
{
	assert(get_block_size(dst) != 0);
	assert(srcSize <= get_block_size(dst));

	int posPtr = 0;

	for (int i = 0; i < blockSize; i++)
	{
		block_t *currBlock = &(allocMemBlocks[i]);

		if (currBlock->address == dst)
		{
			// Traverse the memory block chain
			do
			{

				int minSize = 0;
				if (currBlock->size > srcSize)
				{
					minSize = srcSize;
				}
				else
				{
					minSize = currBlock->size;
				}
				memcpy(heap + currBlock->address, buffer + posPtr, minSize); // Write mem from buffer to heap
				posPtr += currBlock->size;
				srcSize -= currBlock->size;
				currBlock = currBlock->next;
			} while (currBlock != NULL && srcSize > 0);

			return 1;
		}
	}

	return 0;
}