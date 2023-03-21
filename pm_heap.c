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
#define disk_capacity 20
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int times[HEAP_SIZE] = {0};
char *requestMemoryInDisk(int pageId);
intptr_t getLeastRecentlyUsedPageId();
int pm_malloc_to_disk(intptr_t pageId, char byteArray[]);
void updateTimeArray(int address);
void deleteFrameFromDisk(int pageId);
int diskPageId = 0;
int timeCounter = 0;
char *removeLeading(char *str);
updateDisk(int pageId);
int fileOpenFlag = 0;
FILE *f ;

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
	block_t *currBlock = NULL;
	char searchedData[9] = {0};
	char *retValue;

	intptr_t address = pageId;
	int j = 0;
	if (address < HEAP_SIZE)
	{
		for (int i = 0; i < blockSize; i++)
		{
			currBlock = &(allocMemBlocks[i]);

			if (currBlock->address == address && currBlock->isFree == 0)
			{
				printf("Searched data is found in main memory at pageId %d \n", currBlock->address);
				for (int i = pageId; i < (pageId + 4); i++)
				{
					searchedData[j] = heap[i];
					j++;
				}
				searchedData[j] = '\0';
				updateTimeArray(pageId);
				return searchedData;
			}
		}
	}

	else
	{
		// page fault happens
		printf("Data not found in main memory\n");
		char *retValue = requestMemoryInDisk(pageId);
		for (int i = 0; i < 5; i++)
		{
			searchedData[i] = *(retValue + i);
		}
		// if the main memory is not full add the retrieved data above
		// from secondary memory to main memory
		if (searchedData[0] != NULL && (blockSize < MAX_BLOCK))
		{
			struct arg_struct_malloc mallocdata;
			mallocdata.values = searchedData;
			mallocdata.size = 4;
			pm_malloc(&mallocdata);
		}
		else
		{
			// since no space in main memory we need to replace an exisiting least recently used page
			// from the main memory.
			int k = 0;
			int freePageId;
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
			printf("The least recently used data is %s\n", data);
			int disk_address = pm_malloc_to_disk(address, data);
			struct arg_struct_free_dump toFreePageId;
			toFreePageId.pageId = freePageId;
			pm_free(&toFreePageId);
			struct arg_struct_malloc mallocdata;
			mallocdata.values = searchedData;
			mallocdata.size = 4;
			pm_malloc(&mallocdata);
		}
	}
	return searchedData;
}

intptr_t getLeastRecentlyUsedPageId()
{
	intptr_t leastRecentlyUsedPage = times[0];
	int leastTime = 0;
	for (int i = 0; i < HEAP_SIZE; i += 4)
	{
		if (times[i] < leastRecentlyUsedPage)
		{
			leastRecentlyUsedPage = times[i];
			leastTime = i;
		}
	}
	return leastTime;
}


int pm_malloc_to_disk(intptr_t pageId, char byteArray[4])
{
	int index = MAX_BLOCK * 4 + diskPageId;
	if (fileOpenFlag == 0){
		f = fopen("backing_store.csv", "w");
		fileOpenFlag = 1;
	}else{
		f = fopen("backing_store.csv", "a");
	}
	
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	// every record will have a boolean false by default which indicates it has not been read
	// or retrieved
	fprintf(f, "%ld, %s\n", index, byteArray);
	printf("Data %s saved to disk at address %ld \n", byteArray, index);
	diskPageId = diskPageId + 4;
	fclose(f);
	return index;
}

char *requestMemoryInDisk(int pageId)
{
	printf("Reading the backing_store.. \n");

	f = fopen("backing_store.csv", "r");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	char buffer[10];
	int row = 0;
	int column = 0;
	char searchedData[4];
	bool found = false;
	

	while (fgets(buffer, 10, f))
	{
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
				for (int i = 1; i < strlen(token) - 1; i++)
				{
					searchedData[j] = token[i];
					j++;
				}
				printf("Found data in disk is: %s\n", searchedData);
				return searchedData;
			}
			token = strtok(NULL, ",");
			column++;
		}
	}
	fclose(f);
	fflush(f);
	return searchedData;
}



	void updateTimeArray(int address)
	{

		timeCounter = timeCounter + 1;
		timeCounter = timeCounter + times[address];
		times[address] = timeCounter;
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
			struct arg_struct_malloc *args = arguments;
			int size = args->size;
			char byteArray[size];
			printf("Contents of the array are: ");
			memcpy(byteArray, args->values, size);
			for (int i = 0; i < size; i++)
			{
				printf("%c", byteArray[i]);
			}	
			printf("\n");
			block_t *startBlock = NULL;
			block_t *prevBlock = NULL;
			int arrayPtr = 0;
			int hasStarted = 0;

			// Iterate through all existing memory blocks
			for (int i = 0; i < blockSize; i++)
			{
				block_t *currBlock = &(allocMemBlocks[i]);
				// If current mem block is free
				if (currBlock->isFree)
				{

					// If current memory block has smaller size
					if (currBlock->size < size)
					{
						currBlock->isFree = 0; // Set current block to not free
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
						return startBlock->address;
					}
					// If current memory block is equal in size
					else
					{
						currBlock->isFree = 0; // Set current block to not free
						memcpy(heap + currBlock->address, byteArray, size);
						currBlock->next = NULL; // Nullify chain
						blockSize = blockSize + 1;
						updateTimeArray(currBlock->address);
						pthread_mutex_unlock(&lock);
						return currBlock->address; // Return address
					}
				}
			}
			block_t newBlock;
			newBlock.isFree = 0; // Set block to not free
			newBlock.address = get_max_addr();
			newBlock.size = size;
			newBlock.next = NULL;
			allocMemBlocks[blockSize++] = newBlock;
			memcpy(heap + newBlock.address, byteArray + arrayPtr, size);
			updateTimeArray(newBlock.address);

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
			memcpy(byteArray, args->values, size);
			// int index = MAX_BLOCK * 4 + diskPageId;
			int index = pm_malloc_to_disk(index, byteArray);
			// diskPageId = diskPageId + 4;

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
		fflush(stdout);

		for (int i = 0; i < size; i++)
		{
			printf(" %c |", heap[i]);
		}
		printf("\n");
		pthread_mutex_unlock(&lock);
	}

	