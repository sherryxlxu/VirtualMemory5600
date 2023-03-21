/* pm_heap.h  */

/*
 * pm_heap.h / Add Thread Safety
 *
 * Xiaoliang Xu / CS5600 / Northeastern University
 * Spring 2023 / Feb 28, 2023
 *
 */


#ifndef PM_HEAP_H
#define PM_HEAP_H

#include <stddef.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>

/* size of the pm-heap in bytes */
#define PM_HEAP_SIZE 10 * 1024 * 1024

/* size of a single page in bytes */
#define PM_PAGE_SIZE 1024 *1024

#define PAGE_NUM 10

#define VIRTUAL_NUM 30

//heap size
static char phys_mem[PM_HEAP_SIZE];
// static int page_num = PM_HEAP_SIZE/PM_PAGE_SIZE;
static int phys_page[PAGE_NUM];
static int virtual_page[VIRTUAL_NUM];
static int time_page[PAGE_NUM];
// static unsigned char pm_heap [PM_HEAP_SIZE];
// static char phys_mem[PM_HEAP_SIZE];
static pthread_mutex_t pm_mutex;
static pthread_mutex_t queue_mutex;
static pthread_mutex_t access_mutex;
int counter;
int removeFile(int index);
int fetchContent(int index);
int *access_page(int num);
// static int page_num=PM_HEAP_SIZE/PM_PAGE_SIZE;
// static int virtual_num = 2 * PM_HEAP_SIZE/PM_PAGE_SIZE;
void initialize_virtual();
void *pm_malloc(size_t size);
void *pm_free(void *ptr);
void *pm_swap_helper(swap_slot,index_swap,value);
void storeArray(int index, int value);
void *pm_malloc_wrapper(void *arg);
void *pm_free_wrapper(void *arg);

#endif
