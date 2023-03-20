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
int times[HEAP_SIZE] = {0};
char *requestMemoryPageInDisk(int pageId);
intptr_t getLeastRecentlyUsedPageId();
void pm_malloc_to_disk(intptr_t pageId, char byteArray[]);
void updateTimeArray(int address);
int diskPageId = 0;
int timeCounter = 0;
char *removeLeading(char *str);

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
	int pageId;
};

char heap[HEAP_SIZE] = {0};
block_t allocMemBlocks[MAX_BLOCK];

int blockSize = 0;

char *requestMemoryPageInMainMemory(intptr_t pageId)
{

	// pthread_mutex_lock(&lock);
	// struct arg_struct_free_dump *args = arguments;
	// printf("test");
	block_t *currBlock = NULL;
	char searchedData[9] = {0};
	char *retValue;
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
				updateTimeArray(pageId);
				// for(int i = 0; i < 5; i++)
				// 	{
				// 	printf("%c", searchedData[i]);
				// 	}
				// pthread_mutex_unlock(&lock);
				// printf("Inside : %s \n", searchedData);
				printf("Size of the searched data from main memory is: %d", strlen(searchedData));
				return searchedData;
			}
		}
	}

	else
	{
		// page fault happens
		printf("Data not found in main memory\n");
		char *retValue = requestMemoryPageInDisk(pageId);
		for (int i = 0; i < 5; i++)
		{
			searchedData[i] = *(retValue + i);
			// printf("%c\n", searchedData[i]);
		}
		

		// if the main memory is not full add the retrieved data above
		// from secondary memory to main memory
		
		if (searchedData[0] != NULL && (blockSize < MAX_BLOCK))
		{
			printf("Entering if block");
			struct arg_struct_malloc mallocdata;
			mallocdata.values = searchedData;
			mallocdata.size = 4;
			pm_malloc(&mallocdata);
		}
		else
		{
			// since no space in main memory we need to replace an exisiting least recently used page
			// from the main memory.
			printf("Entering else block");
			int k = 0;
			int freePageId ;
			intptr_t pageIdToReplace = getLeastRecentlyUsedPageId();
			freePageId = pageIdToReplace;
			printf("Page id to replace from main memory is %d :\n", pageIdToReplace);
			char data[4] = {0};
			for (int i = pageIdToReplace; i < (pageIdToReplace + 4); i++)
			{
				data[k] = heap[i];
				k++;
			}
			data[k] = '\0';
			printf("Data to be saved to disk due to least recently used %s\n", data);
			pm_malloc_to_disk(address, data);
			struct arg_struct_free_dump toFreePageId;
			toFreePageId.pageId = freePageId;
			// printf("Sending page id to free is %d \n", freePageId);
			pm_free(&toFreePageId);
			struct arg_struct_malloc mallocdata;
			mallocdata.values = searchedData;
			mallocdata.size = 4;
			pm_malloc(&mallocdata);
		}
	}

	// add the least recently used page to disk and the searched page to
	// back to main memory in place of the recently used memory.

	// updateTimeArray(address, currBlock);
	//  pthread_mutex_unlock(&lock);
	return searchedData;
}
intptr_t getLeastRecentlyUsedPageId()
{
	intptr_t leastRecentlyUsedPage = times[0];
	int leastTime = 0;
	// 	printf("Time %d at %d \n" , times[0], 0);
	// printf("LRU times array size is %d,%d", strlen(times), sizeof(times));
	for (int i = 0; i < HEAP_SIZE ; i+=4)
	{
		printf("Time %d at %d \n", times[i], i);
		if (times[i] < leastRecentlyUsedPage)
		{
			leastRecentlyUsedPage = times[i];
			leastTime = i;
		}
	}
	return leastTime;
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
	// for (int i = 0; i < 4; i++)
	// {
	// 	printf("%c", byteArray[i]);
	// }

	// printf("Starting pm_malloc! %d \n", blockSize);
	// printf("\n");
	fprintf(f, "%ld, %s\n", pageId, byteArray);
	printf("Data saved to csv file \n");
	fclose(f);
}

char *requestMemoryPageInDisk(int pageId)
{
	printf("Reading the backing_store.. \n");

	f = fopen("backing_store.csv", "r");
	// FILE *write = fopen("backing_store.csv", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	char buffer[80];
	int row = 0;
	int column = 0;
	char searchedData[4];
	bool found = false;
	while (fgets(buffer, 80, f))
	{
		// printf("read the inside the csv \n");
		column = 0;
		// If you need all the values in a row
		char *token = strtok(buffer, ", ");
		int j = 0;
		while (token)
		{
			if (column == 0 && atoi(token) == pageId)
			{
				found = true;
			}

			if (column == 1 && found == true)
			{
				for(int i = 1 ; i < strlen(token) - 1; i++)
				{
					searchedData[j] = token[i];
					j++;
				}
				printf("Found data in disk is: %s\n", searchedData);
				printf("Found data in disk is: 1 - %c 2 - %c 3 - %c 4 - %c 5 - %c 6 - %c\n", token[0], token[1], token[2], token[3], token[4],token[5]);
				printf("size of data from disk is : %d\n", strlen(token));
			
				// printf("Found trimmed data  in disk is: %s\n", str1);
				return searchedData;
			}
			// printf("%s\n", token);
			token = strtok(NULL, ",");
			column++;
		}
	}
	fclose(f);
	return searchedData;
}

// char *removeLeading(char *str)
// {
// 	int idx = 0, j, k = 0;

// 	char str1[4] = {0};
// 	// Iterate String until last
// 	// leading space character
// 	while (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\n')
// 	{
// 		idx++;
// 	}

// 	// Run a for loop from index until the original
// 	// string ends and copy the content of str to str1
// 	for (j = idx; str[j] != '\0'; j++)
// 	{
// 		str1[k] = str[j];
// 		k++;
// 	}

// 	// Insert a string terminating character
// 	// at the end of new string
// 	str1[k] = '\0';

// 	// Print the string with no whitespaces
// 	printf("%s", str1);
// 	return str1;
// }

void updateTimeArray(int address)
{
	// time_t seconds;
	timeCounter = timeCounter + 1;
	timeCounter = timeCounter + times[address];
	times[address] = timeCounter;
	printf("The time at address %d is %d",address, times[address] );
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
		printf("Size of the malloc array is: %d", size);
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
					printf("Entering first condition");
					memcpy(heap + currBlock->address, byteArray + arrayPtr, currBlock->size);
					updateTimeArray(currBlock->address);
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
					printf("Entering second condition");
					memcpy(heap + currBlock->address, byteArray + arrayPtr, size);

					// "Split" original block into a new block
					updateTimeArray(currBlock->address);
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
					printf("Entering third condition");
					memcpy(heap + currBlock->address, byteArray, size);
					currBlock->next = NULL; // Nullify chain
					printf("Current blocksize is :%d", blockSize);
					allocMemBlocks[blockSize++] = *currBlock;
					printf("Current blocksize is :%d", blockSize);
					updateTimeArray(currBlock->address);
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
		printf("Entering fourth condition");
		memcpy(heap + newBlock.address, byteArray + arrayPtr, size);
		printf("Current blocksize is :%d", blockSize);
		// time_t seconds;
		updateTimeArray(newBlock.address);
		// times[newBlock.address] = timeCounter +;
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
	intptr_t address = args->pageId;
	// assert(blockSize < MAX_BLOCK);
	printf("Page id to be freed from main memory is: %d \n", address);
	for (int i = 0; i < blockSize; i++)
	{
		block_t *currBlock = &(allocMemBlocks[i]);

		if (currBlock->address == address && currBlock->isFree == 0)
		{
			while (currBlock != NULL)
			{
				currBlock->isFree = 1;								   // Set memory block to free
				memset(heap + currBlock->address, 0, currBlock->size); // Nullify the bytes
				currBlock = currBlock->next;
				blockSize = blockSize - 1; // Set current to next mem block in chain
			}
			pthread_mutex_unlock(&lock);
			printf("Current blocksize is :%d", blockSize);
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
	int size = args->pageId;
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