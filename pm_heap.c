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

void initialize_virtual(){
  pthread_mutex_lock(&queue_mutex);
  int length;
  length = sizeof(virtual_page)/sizeof(virtual_page[0]);
  // printf("length: %d\n, %d\n, %d\n",sizeof(virtual_page),sizeof(virtual_page[0]), length);
  // printf("PAGE_NUM:%d, %d, %d\n", PM_HEAP_SIZE, PM_PAGE_SIZE, (PM_HEAP_SIZE/PM_PAGE_SIZE));
  for(int i = 0; i < length; i++){
    virtual_page[i] = -1;
    printf("virtual_malloc %d: %d\n", i, virtual_page[i]);
  }
  pthread_mutex_unlock(&queue_mutex);
}
void *pm_malloc(size_t size){
  //find an available space in virtual page
  //size of virtual memory is twice as physical memory
  //virtual memory: index is page number, content is the time that allocated
  int i = 0;
  void *slot = NULL;
  //counter for calculate the order
  // int counter=0;
  printf("counter1: %d\n", counter);
  if (counter > 9){
    //free helper
    //update virtual Memory
    //get the index from time_page
    int index_swap = time_page[0];
    //move the index to swap space
    for (int j = 0; j < PAGE_NUM; j++) {
      if (virtual_page[j + VIRTUAL_NUM-PAGE_NUM] < 0){
        virtual_page[j + VIRTUAL_NUM-PAGE_NUM] = index_swap;
        break;
        //todo: put index_swap into swapfile
      }
    }
    //free time_page
    void *swap_slot = phys_mem + PM_PAGE_SIZE * 0;

    pm_swap_helper(swap_slot);
    // pm_malloc_helper()
    
  }
  printf("counter2: %d\n", counter);
  while (i < (VIRTUAL_NUM-PAGE_NUM)) {
    if(counter > 9){
      
      break;
    }
    if (virtual_page[i] == -1){
      //update virtual page
      virtual_page[i] = counter;
      //update time page according to virtual page
      time_page[counter] = i;
      //update physical page according to time_page
      phys_page[counter] = 1;
      counter ++;
      printf("counter3: %d\n", counter);
      slot = phys_mem + PM_PAGE_SIZE * i;
      break;
      
    }
    
    i++;

  }
  //virtual memory is full
  if (i == (VIRTUAL_NUM-PAGE_NUM)){
    printf("Virtual memory is full.\n");
    return NULL;
  }

  printf("counter: %d\n", counter);
  // int length = sizeof(virtual_page)/sizeof(virtual_page[0]);
  for (int m=0; m < VIRTUAL_NUM ; m++){
    printf("virtual_malloc %d: %d\n", m, virtual_page[m]);
  }
  for (int m=0; m < PAGE_NUM ; m++){
    printf("time_malloc %d: %d\n", m, time_page[m]);
  }
  for (int m=0; m < PAGE_NUM ; m++){
    printf("phys_malloc %d: %d\n", m, phys_page[m]);
  }
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
  if (counter < 0){
    return;
  }
  int page_index = (ptr - (void*)phys_mem) / (1024*1024);   /* find how many pages need to be freed  */
  printf("page_index: %d\n",page_index);
  if(virtual_page[page_index] >= 0){
    printf("page_index: %d\n",page_index);
    int temp = virtual_page[page_index];
    virtual_page[page_index] = -1;
    time_page[temp] = -1;

    for (int j= temp; j < PAGE_NUM - 1; j ++){
      int temp_a =time_page[j];
      time_page[j] =time_page[j+1];
      time_page[j+1]=temp_a;
      printf("time_page: %d\n",time_page[j+1]);
    }

    for (int i = 0; i < PAGE_NUM; i++){
    if (time_page[i] == -1){
      continue;
    }
    int index = time_page[i];
    virtual_page[index] = i;
  }
    

  }

   for(int i = 0; i < PAGE_NUM; i++){
      if (time_page[i] == -1){
        phys_page[i] = 0;
      }
    }
    int new_count;
    for(int i = 0; i < PAGE_NUM; i++){
      if (phys_page[i] == 1){
        new_count++;
      }
    }
    counter = new_count;
  // for(int i = 0; i < VIRTUAL_NUM - PAGE_NUM; i++){
  //   if(virtual_page[i] == page_index){
  //     printf("page_index: %d\n",page_index);
  //     int temp = page_index;
  //     virtual_page[temp] = -1;
  //     time_page[i] = -1;
  //     for (int j = i; j < PAGE_NUM - 1; j++){
  //       int temp_a = time_page[j];
  //       time_page[j] = time_page[j + 1];
  //       time_page[j+1] = temp;
  //     // printf("time_page: %d\n",time_page[i+1]);
  //     } 
  //     phys_page[i] = 0;
  //     counter --;
  //     break;
  //   }
    
  // }
  //update virtual memory
  
 
  // int index_free = page_index; 
  // phys_page[index_free] = 0;
  // counter -= 1;
  // int free_page = virtual_page[page_index];
  // time_page[free_page] = -1;
  // phys_page[free_page] = 0;
  // virtual_page[page_index] = -1;
  // // virtual_page[free_page] = -1;
  // // time_page[index_free] = -1;
  // // printf("index_free: %d\n",index_free);
  // // printf("free_page: %d\n",free_page);
  // for (int i = free_page; i < PAGE_NUM - 1; i++){
  //   int temp = time_page[i];
  //   time_page[i] = time_page[i + 1];
  //   time_page[i+1] = temp;
  //   // printf("time_page: %d\n",time_page[i+1]);
  // }
  // //update virtual memory
  // for (int i = 0; i < PAGE_NUM; i++){
  //   if (time_page[i] == -1){
  //     continue;
  //   }
  //   int index = time_page[i];
  //   virtual_page[index] = i;
  // }
  // counter -=1;
  // phys_page[free_page]=0;

  printf("counter: %d\n", counter);
  for (int m=0; m < VIRTUAL_NUM ; m++){
    printf("virtuafree %d: %d\n", m, virtual_page[m]);
  }
  for (int m=0; m < PAGE_NUM ; m++){
    printf("time_free %d: %d\n", m, time_page[m]);
  }
  for (int m=0; m < PAGE_NUM ; m++){
    printf("phys_free %d: %d\n", m, phys_page[m]);
  }

}

void* pm_swap_helper(void *ptr){
  if (ptr == NULL){
    return;
  }
  if (counter < 0){
    return;
  }
  int page_index = (ptr - (void*)phys_mem) / PM_PAGE_SIZE;   /* find how many pages need to be freed  */
 
  int index_free = page_index; 
  int free_page = time_page[index_free]; 
  virtual_page[free_page] = -4;
  time_page[index_free] = -1;
  printf("index_free: %d\n",index_free);
   printf("free_page: %d\n",free_page);
  for (int i = index_free; i < PAGE_NUM - 1; i++){
    int temp = time_page[i];
    time_page[i] = time_page[i + 1];
    time_page[i+1] = temp;
   
  }
  //update virtual memory
  for (int i = 0; i < PAGE_NUM; i++){
    if (time_page[i] == -1){
      continue;
    }
    int index = time_page[i];
    virtual_page[index] = i;
  }
  if (ptr == NULL){
    return;
  }
  if (counter < 0){
    return;
  }
  // int page_index = (ptr - (void*)phys_mem) / (1024*1024);   /* find how many pages need to be freed  */
 
  // // int index_free = page_index; 
  // // phys_page[index_free] = 0;
  // counter -= 1;
  // int free_page = virtual_page[page_index];
  // time_page[free_page] = -1;
  // phys_page[page_index] = 1;
  // virtual_page[page_index] = -1;
  // // virtual_page[free_page] = -1;
  // // time_page[index_free] = -1;
  // // printf("index_free: %d\n",index_free);
  // // printf("free_page: %d\n",free_page);
  // for (int i = free_page; i < PAGE_NUM - 1; i++){
  //   int temp = time_page[i];
  //   time_page[i] = time_page[i + 1];
  //   time_page[i+1] = temp;
  //   // printf("time_page: %d\n",time_page[i+1]);
  // }
  // //update virtual memory
  // for (int i = 0; i < PAGE_NUM; i++){
  //   if (time_page[i] == -1){
  //     continue;
  //   }
  //   int index = time_page[i];
  //   virtual_page[index] = i;
  // }
  counter -=1;
  phys_page[index_free]=1;

  printf("counter: %d\n", counter);
  for (int m=0; m < VIRTUAL_NUM ; m++){
    printf("virtuafree %d: %d\n", m, virtual_page[m]);
  }
  for (int m=0; m < PAGE_NUM ; m++){
    printf("time_free %d: %d\n", m, time_page[m]);
  }
  for (int m=0; m < PAGE_NUM ; m++){
    printf("phys_free %d: %d\n", m, phys_page[m]);
  }

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





