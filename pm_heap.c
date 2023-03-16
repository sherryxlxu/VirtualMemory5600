/* pm_heap.c  */

/*
 * pm_heap.c / Add Thread Safety
 *
 * Xiaoliang Xu / CS5600 / Northeastern University
 * Spring 2023 / Feb 28, 2023
 *
 */

/*
 * This program is a custom heap and it is divided into several pages.
 * pm_malloc(size_t size) allocate size into an available page in heap
 * pm_free(*ptr) free the pages of memory that allocated from ptr.
 * pm_malloc_wrapper(void *arg) is a helper function for pthread_create
 * pm_free_wrapper(void *arg) is a helper function for pthread_create
 *
 */

#include "pm_heap.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXSIZE 10

int queue[MAXSIZE];
int front = -1;
int rear = -1;
int size = -1;
pthread_t queue_thread;
void enqueue(int value){
  pthread_mutex_lock(&queue_mutex);
  if(size < MAXSIZE){
    if (size < 0){
      queue[0] = value;
      front = rear = 0;
      size = 1;

    } else if (rear == MAXSIZE - 1){
      queue[0] = value;
      rear = 0;
      size ++;
    } else {
    queue[rear + 1] = value;
    rear++;
    size ++;
  }
  } else{
    printf("queue is full\n");
  }
  pthread_mutex_unlock(&queue_mutex);
}
void swap(int value){
  pthread_mutex_lock(&queue_mutex);
  int temp;
  if (size < 0){
    printf("queue is empty\n");
  } else{
  temp = queue[front];
  queue[front] = queue[value];
  queue[value] = temp;
  dequeue();
  }
  pthread_mutex_unlock(&queue_mutex);
}
int dequeue(){
  // pthread_mutex_lock(&pm_mutex);
  if (size < 0){
    printf("queue is empty\n");
  } else {
    size --;
    front ++;
  }
  // pthread_mutex_unlock(&queue_mutex);
}

void display() {
  pthread_mutex_lock(&queue_mutex);
    int i;
    if(rear>=front) {
        for(i=front;i<=rear;i++){
            printf("%d\n",queue[i]);
        }
    } else
    {
        for(i=front;i<MAXSIZE;i++){
            printf("%d\n",queue[i]);
        }
        for(i=0;i<=rear;i++){
            printf("%d\n",queue[i]);
        }
    }
  pthread_mutex_unlock(&queue_mutex);
}

/*
  * pm_malloc()-allocate size into an available page in heap
  * 
  * size: size need to be allocated
  *
  */

void *pm_malloc(size_t size){
  

  
  if(size <= 0 || size > PM_HEAP_SIZE) {  /* check if size is suitable for heap  */
    return NULL;
  }
  int nums = size / PM_PAGE_SIZE;         /* calculate how many page does this size need  */

  if (size % PM_PAGE_SIZE != 0){
    nums += 1;
  }
  int i = 0;                              /* iterate to an available page  */
  void *slot = NULL;
  while (i < page_num){
    if(pm_page[i] == 1){                  /* if the page is allocated, move to next one  */
      i++;
      continue;
    }
    if (i + nums > page_num) {            /* if current page index plus additional page needed is greater than page num, break*/
      break;
    }                                     /* pm_page[i] is now empty  */
    int count = 0;
    for(int j = i; j < i + nums; j++){    /* find continuous empty blocks for size  */
      if (pm_page[j] != 0){
        break;
      }
      count ++;
    }
    if (count != nums){                   /* not enough pages for insert size, update index i to continue looking for available pages  */
      i += count;
      if (i>=page_num){                   /* not enough pages for insert size, allocation failed  */
        return NULL;
      }
      continue;
    }
    for (int k = i; k < i + count; k++){ /* update the allocation of pm_page[] according to count  */
      pm_page[k] = 1;
      pthread_create(&queue_thread,NULL,enqueue,k);
      pthread_join(queue_thread,NULL);
    }
    pm_page[i] = nums;                   /* pm_page[i] store the nums, so we could know how many pages need to be freed */
    slot = pm_heap +(PM_PAGE_SIZE * i);
    break;
  }
  int available = 0;                    /* check the situation of pages  */
  int length = sizeof(pm_page) / sizeof(pm_page[0]);
  for(int i = 0; i < length;i++){
    printf("pm_page_malloc %d: %d\n",i, pm_page[i]);
    if (pm_page[i] == 0){
      available++;
    } 
  }
  display();
  // sleep(1);
  // return available pages;
  printf("Available pages in heap after malloc: %d\n",available);

  return slot;

}

 /*
  * pm_free()-free a block of memory to the pm-heap
  * 
  * ptr: the pointer to the memory to free
  *
  */
 void* pm_free(void *ptr){
  if (ptr == NULL){
    return;
  }
  int page_index = (ptr - (void*)pm_heap) / PM_PAGE_SIZE;   /* find how many pages need to be freed  */
  int i;
  for (i = 0; i < pm_page[page_index]; i++){                /* free all the pages that have been allocated before  */
      pm_page[i+page_index] = 0;
      for (int j = 0; j <sizeof(queue)/sizeof(queue[0]);j++){
        if (queue[j] == i+page_index){
          pthread_create(&queue_thread,NULL,swap,j);
          pthread_join(queue_thread,NULL);
        }
      }
  }
  int available = 0;
  int length = sizeof(pm_page) / sizeof(pm_page[0]);        /* check the situation of pages  */
  for(int i = 0; i < length;i++){
    printf("pm_page_freed: %d\n",pm_page[i]);
    if (pm_page[i] == 0){
      available++;
    }
  }
  display();
  // return count;
  printf("Available pages in heap after free: %d\n",available);
}

 /*
  * pm_malloc_wrapper()-helper function for pthread_create
  * 
  * arg: size that would be used in pm_malloc()
  *
  */
void *pm_malloc_wrapper(void *arg) {    
    pthread_mutex_lock(&pm_mutex);
    // Cast the argument to a size_t
    size_t size = (size_t) arg;
    // Call the pm_malloc() function
    void *ptr = pm_malloc(size);
    pthread_mutex_unlock(&pm_mutex);
    // Return the result of the pm_malloc() function
    return ptr;
}

 /*
  * pm_free_wrapper()-helper function for pthread_create
  * 
  * arg: pointer that would be used in pm_free()
  *
  */

void *pm_free_wrapper(void *arg) {    
    pthread_mutex_lock(&pm_mutex);
    pm_free(arg);
    pthread_mutex_unlock(&pm_mutex);
}





