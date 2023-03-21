#include <stdio.h>
#include "pm_heap.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

struct arg_struct_malloc
{
	char *values;
	int size;
};

struct arg_struct_free_dump
{
	int size;
};

void print_buffer(char buffer[], int size)
{
	printf("Buffer: ");

	for (int i = 0; i < size; i++)
	{
		printf(" %c |", buffer[i]);
	}

	printf("\n");
}

int main()
{

	char data[4] = {'a', 'b', 'c', 'd'};
	char data1[4] = {'e', 'f', 'g', 'h'};
	char data2[4] = {'i', 'j', 'k', 'l'};
	char data3[4] = {'m', 'n', 'o', 'p'};
	char data4[4] = {'q', 'r', 's', 't'};
	char data5[4] = {'u', 'v', 'w', 'x'};
	char data6[4] = {'y', 'z', 'A', 'B'};
	char data7[4] = {'C', 'D', 'E', 'F'};
	char data8[4] = {'G', 'H', 'I', 'J'};
	char data9[4] = {'K', 'L', 'M', 'N'};
	char data10[4] = {'O', 'P', 'Q', 'R'};
	char data11[4] = {'S', 'T', 'U', 'V'};
	char data12[4] = {'W', 'X', 'Y', 'Z'};

	// {'d', 'e', 'f'},
	// {'g', 'h', 'i'},
	// {'j', 'k', 'l'},
	// {'m', 'n', 'o'},
	// {'p', 'q', 'r'}};

	intptr_t address;

	pthread_t tid[19];

	struct arg_struct_malloc mallocdata1;
	mallocdata1.values = data;
	mallocdata1.size = 4;
	pthread_create(&(tid[0]), NULL, &pm_malloc, (void *)&mallocdata1);
	pthread_join(tid[0], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata2;
	mallocdata2.values = data1;
	mallocdata2.size = 4;
	pthread_create(&(tid[1]), NULL, &pm_malloc, (void *)&mallocdata2);
	pthread_join(tid[1], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata3;
	mallocdata3.values = data2;
	mallocdata3.size = 4;
	pthread_create(&(tid[2]), NULL, &pm_malloc, (void *)&mallocdata3);
	pthread_join(tid[2], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata4;
	mallocdata4.values = data3;
	mallocdata4.size = 4;
	pthread_create(&(tid[3]), NULL, &pm_malloc, (void *)&mallocdata4);
	pthread_join(tid[3], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata5;
	mallocdata5.values = data4;
	mallocdata5.size = 4;
	pthread_create(&(tid[4]), NULL, &pm_malloc, (void *)&mallocdata5);
	pthread_join(tid[4], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata6;
	mallocdata6.values = data5;
	mallocdata6.size = 4;
	pthread_create(&(tid[5]), NULL, &pm_malloc, (void *)&mallocdata6);
	pthread_join(tid[5], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata7;
	mallocdata7.values = data6;
	mallocdata7.size = 4;
	pthread_create(&(tid[6]), NULL, &pm_malloc, (void *)&mallocdata7);
	pthread_join(tid[6], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata8;
	mallocdata8.values = data7;
	mallocdata8.size = 4;
	pthread_create(&(tid[7]), NULL, &pm_malloc, (void *)&mallocdata8);
	pthread_join(tid[7], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata9;
	mallocdata9.values = data8;
	mallocdata9.size = 4;
	pthread_create(&(tid[8]), NULL, &pm_malloc, (void *)&mallocdata9);
	pthread_join(tid[8], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata10;
	mallocdata10.values = data9;
	mallocdata10.size = 4;
	pthread_create(&(tid[9]), NULL, &pm_malloc, (void *)&mallocdata10);
	pthread_join(tid[9], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata11;
	mallocdata11.values = data10;
	mallocdata11.size = 4;
	pthread_create(&(tid[11]), NULL, &pm_malloc, (void *)&mallocdata11);
	pthread_join(tid[11], (void *)&address);
	printf("the pageId is : %ld \n", address);

	// this set of threads will print the entire heap so far

	struct arg_struct_free_dump dump;
	dump.size = 40;
	pthread_create(&(tid[10]), NULL, &dump_heap, (void *)&dump);
	pthread_join(tid[10], NULL);

	struct arg_struct_malloc mallocdata12;
	mallocdata12.values = data11;
	mallocdata12.size = 4;
	pthread_create(&(tid[12]), NULL, &pm_malloc, (void *)&mallocdata12);
	pthread_join(tid[12], (void *)&address);
	printf("the pageId is : %ld \n", address);

	struct arg_struct_malloc mallocdata13;
	mallocdata13.values = data12;
	mallocdata13.size = 4;
	pthread_create(&(tid[13]), NULL, &pm_malloc, (void *)&mallocdata13);
	pthread_join(tid[13], (void *)&address);
	printf("the pageId is : %ld \n", address);

	//retrive the data from the main memory using the pageId
	intptr_t searchPageId = 8;
	char *retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from memory at page id %d is : %c%c%c%c%c\n",searchPageId, retValue[0], retValue[1], retValue[2], retValue[3]);
	//data retreival from disk

	searchPageId = 44;
	retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from disk at frame id %d is : %c%c%c%c%c\n",  searchPageId, retValue[0], retValue[1],
		   retValue[2], retValue[3], retValue[4], retValue[5], retValue[6], retValue[7], retValue[8]);
	

	searchPageId = 40;
	retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from disk at frame id %d is : %c%c%c%c%c\n", searchPageId, retValue[0], retValue[1],
		   retValue[2], retValue[3], retValue[4], retValue[5], retValue[6], retValue[7], retValue[8]);

	pthread_create(&(tid[14]), NULL, &dump_heap, (void *)&dump);
	pthread_join(tid[14], NULL);

	searchPageId = 4;
	retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from memory at pageId 4 is : %c%c%c%c%c\n", retValue[0], retValue[1], retValue[2], retValue[3]);

	searchPageId = 48;
	retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from disk at frame id %d is : %c%c%c%c%c\n", searchPageId, retValue[0], retValue[1],
		   retValue[2], retValue[3], retValue[4], retValue[5], retValue[6], retValue[7], retValue[8]);

	pthread_create(&(tid[15]), NULL, &dump_heap, (void *)&dump);
	pthread_join(tid[15], NULL);

	// // freeing memory will straight away load the data from the mentioned pageId to main memory
	struct arg_struct_free_dump free;
	free.size = 36;
	pthread_create(&(tid[16]), NULL, &pm_free, (void *)&free);
	pthread_join(tid[16], NULL);

	pthread_create(&(tid[17]), NULL, &dump_heap, (void *)&dump);
	pthread_join(tid[17], NULL);

	searchPageId = 0;
	retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from disk at page id %d is : %c%c%c%c%c\n", searchPageId, retValue[0], retValue[1],
		   retValue[2], retValue[3], retValue[4], retValue[5], retValue[6], retValue[7], retValue[8]);

	searchPageId = 52;
	retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from disk at page id %d is : %c%c%c%c%c\n", searchPageId, retValue[0], retValue[1],
		   retValue[2], retValue[3], retValue[4], retValue[5], retValue[6], retValue[7], retValue[8]);

	pthread_create(&(tid[18]), NULL, &dump_heap, (void *)&dump);
	pthread_join(tid[18], NULL);

	pthread_exit(NULL);

	return 0;
}