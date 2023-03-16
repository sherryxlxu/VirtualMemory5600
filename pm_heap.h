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

/* size of the pm-heap in bytes */
#define PM_HEAP_SIZE (10 * 1024 * 1024)

/* size of a single page in bytes */
#define PM_PAGE_SIZE (1024 * 1024)

static int page_num = PM_HEAP_SIZE/PM_PAGE_SIZE;
static int pm_page [PM_HEAP_SIZE/PM_PAGE_SIZE];
static unsigned char pm_heap [PM_HEAP_SIZE];
static char phys_mem[PM_HEAP_SIZE];
static pthread_mutex_t pm_mutex;
static pthread_mutex_t queue_mutex;
void *pm_malloc(size_t size);
void *pm_free(void *ptr);
void *pm_malloc_wrapper(void *arg);
void *pm_free_wrapper(void *arg);

#endif
