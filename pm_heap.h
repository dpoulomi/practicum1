#pragma once

// Type typedefs for heap malloc
typedef struct MemoryBlock block_t;
typedef struct arg_struct_malloc mallocargs;
typedef struct arg_struct_free_dump freeargs;
typedef struct arg_struct_request_page requestPage;
typedef long intptr_t;
typedef char heap_t[10 * 1024 * 1024];
typedef block_t *memblocks_t[10000];

intptr_t pm_malloc(void *arguments);
void pm_free(void *arguments);
void dump_heap(void *arguments);
int read_mem(intptr_t dst, char buffer[], int size);
int write_mem(intptr_t dst, char buffer[], int srcSize);
// char *requestMemoryPageInMainMemory(intptr_t pageId);
char *requestMemoryPageInMainMemory(void *arguments);