/* test_pm_heap.c  */

/*
 * test_pm_heap.c / Add Thread Safety
 *
 * Xiaoliang Xu / CS5600 / Northeastern University
 * Spring 2023 / Feb 21, 2023
 *
 */

/*
 * This program is a to test if the custom heap could be safely accessed from different threads.
 */

#include "pm_heap.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#define NUM_THREADS 20
void* pm_malloc_results[10];


 //Main program
int main() {
  pthread_t threads[NUM_THREADS], threadsFree[NUM_THREADS],threadExtra,queue_thread;

  pthread_mutex_init(&pm_mutex,NULL);
  pthread_mutex_init(&queue_mutex,NULL);

  pthread_create(&queue_thread, NULL, initialize_virtual,NULL);
  pthread_join(queue_thread,NULL);
  

  /*
   * Test Case 1: Allocate all the pages in heap. The available pages after malloc is going to be 0
   */
  for (int i = 0; i < NUM_THREADS; i++) {
    // thread_args[i] = 1;
    pthread_create(&threads[i], NULL, pm_malloc_wrapper,1);
  }
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], &pm_malloc_results[i]); /* Store the pointers in the pm_malloc_result array */
  }

  // /*
  //  * Test Case 2: Allocate additional page as heap is full. The available pages after malloc is going to be 0
  //  */
  // usleep(10000);
  // pthread_create(&threadExtra,NULL,pm_malloc_wrapper,1);
  // pthread_join(threadExtra, NULL);
  // usleep(10000);

  /*
   * Test Case 3: Free random pages in heap. The available pages after free is going to be 6
   */
  for (int i = 1; i < 7; i++){
    pthread_create(&threadsFree[i], NULL, pm_free_wrapper,pm_malloc_results[i]);
  }
  for (int i = 1; i < 7; i++){
    pthread_join(threadsFree[i], NULL);
  }
  usleep(10000);
  pthread_create(&threadExtra,NULL,pm_malloc_wrapper,1);
  pthread_join(threadExtra, NULL);
  usleep(10000);
  /* 
   * Test Case 4: Allocate additional page in heap that is not full. The available pages after malloc is going to be 5
   */
  // usleep(10000);
  // pthread_create(&threadExtra,NULL,pm_free_wrapper,pm_malloc_results[1]);
  // pthread_join(threadExtra, NULL);
  // usleep(10000);
  // printf("%d\n",PAGE_NUM);
  pthread_mutex_destroy(&pm_mutex);
  pthread_mutex_destroy(&queue_mutex);
  return 0;


}