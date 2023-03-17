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
	char data10[4] ={'O', 'P', 'Q', 'R'};
	char data11[4] ={'S', 'T', 'U', 'V'};


		// {'d', 'e', 'f'},
		// {'g', 'h', 'i'},
		// {'j', 'k', 'l'},
		// {'m', 'n', 'o'},
		// {'p', 'q', 'r'}};

	intptr_t address;
	
	pthread_t tid[16];
	
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


	
	//retrive the data from the main memory using the pageId
	intptr_t searchPageId = 8;
	// char searchedData[40];
	char* retValue = requestMemoryPageInMainMemory(searchPageId);
	printf("Data from memory is : %c%c%c%c%c\n", retValue[0], retValue[1], retValue[2], retValue[3], retValue[4]);	
	// data retreival from disk
	
	searchPageId = 44;
	retValue = requestMemoryPageInMainMemory(searchPageId);	
	printf("Data from disk at page id %d is : %c%c%c%c%c\n",searchPageId, retValue[0], retValue[1], 
	retValue[2], retValue[3], retValue[4],retValue[5],retValue[6],retValue[7],retValue[8]);
	// strcpy(searchedData, requestMemoryPageInMainMemory(searchPageId));
	// printf("Data from disk is %s", searchedData);

	searchPageId = 40;
	retValue = requestMemoryPageInMainMemory(searchPageId);	
	printf("Data from disk at page id %d is : %c%c%c%c%c\n",searchPageId, retValue[0], retValue[1], 
	retValue[2], retValue[3], retValue[4],retValue[5],retValue[6],retValue[7],retValue[8]);

	

	
	pthread_create(&(tid[13]), NULL, &dump_heap, (void *)&dump);
	pthread_join(tid[13], NULL);
	

	// trying to add another data to main memory, but due to lack of memory the 
	//data should be saved in the disk.

	//}
	// pthread_t tid1[2];
	// struct arg_struct_free_dump free;
	// // for (int i = 0; i < 12; i += 2)
	// // {

	// 	free.size = 4;
	// 	// this thread will free memory from heap sequentially in the order added
	// 	pthread_create(&(tid1[0]), NULL, &pm_free, (void *)&free);
	// 	pthread_join(&tid1[0], NULL);
	// 	// this set of threads will print the entire heap
	// 	pthread_create(&(tid1[1]), NULL, &dump_heap, (void *)&dump);
	// 	pthread_join(tid1[1], NULL);
	// //}

	// char data1 = {'A', 'B', 'C', 'D'};
	// // malloc, dump, free and dump
	// pthread_t tid2[24];
	// for (int i = 0; i < 24; i += 4)
	// {

	// 	mallocdata1.values = data1[i / 4];
	// 	mallocdata1.size = 3;
	// 	// this thread will allocate memory in heap as per the mallocdata without any overlap
	// 	pthread_create(&(tid2[i]), NULL, &pm_malloc, (void *)&mallocdata1);
	// 	pthread_join(tid2[i], (void *)&address);
	// 	// this set of threads will print the entire heap so far
	// 	pthread_create(&(tid2[i + 1]), NULL, &dump_heap, (void *)&dump);
	// 	pthread_join(tid2[i + 1], NULL);

	// 	free.size = 0;
	// 	// this thread will free memory from heap sequentially in the order added
	// 	pthread_create(&(tid2[i + 2]), NULL, &pm_free, (void *)&free);
	// 	pthread_join(&tid2[i + 2], NULL);
	// 	// this set of threads will print the entire heap
	// 	pthread_create(&(tid2[i + 3]), NULL, &dump_heap, (void *)&dump);
	// 	pthread_join(tid2[i + 3], NULL);
	// }
	pthread_exit(NULL);

	return 0;
}