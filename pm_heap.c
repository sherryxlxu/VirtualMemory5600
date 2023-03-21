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
#include <string.h>

void initialize_virtual(){
  pthread_mutex_lock(&pm_mutex);
  int length;
  length = sizeof(virtual_page)/sizeof(virtual_page[0]);
  // printf("length: %d\n, %d\n, %d\n",sizeof(virtual_page),sizeof(virtual_page[0]), length);
  // printf("PAGE_NUM:%d, %d, %d\n", PM_HEAP_SIZE, PM_PAGE_SIZE, (PM_HEAP_SIZE/PM_PAGE_SIZE));
  for(int i = 0; i < length; i++){
    virtual_page[i] = -1;
    printf("virtual_malloc %d: %d\n", i, virtual_page[i]);
  }
  for(int i = 0; i < PAGE_NUM; i++){
    phys_page[i] = -1;
    printf("phys_init %d: %d\n", i, phys_page[i]);
  }
  pthread_mutex_unlock(&pm_mutex);
}
void *pm_malloc(size_t size){
  if(size <= 0 || size > PM_HEAP_SIZE) {  /* check if size is suitable for heap  */
    return NULL;
  }
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
    int value = phys_page[0];
    // printf("index_swap: %d\n", index_swap);
    // //move the index to swap space
    // for (int j = 0; j < PAGE_NUM; j++) {
    //   if (virtual_page[j + VIRTUAL_NUM-PAGE_NUM] < 0){
    //     virtual_page[j + VIRTUAL_NUM-PAGE_NUM] = index_swap;
    //     storeArray(index_swap,value);
    //     break;
    //   }
    // }
    //free time_page
    void *swap_slot = phys_mem + PM_PAGE_SIZE * 0;
    printf("index_swap %d\n: %d\n",index_swap, value);
    pm_swap_helper(swap_slot,index_swap,value);
    // pm_malloc_helper(size, counter);
    
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
      phys_page[counter] = size;
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
  if(virtual_page[page_index] == -4){
    for(int i = 0; i < PAGE_NUM; i++){
      if (virtual_page[i + VIRTUAL_NUM - PAGE_NUM] == page_index){
        removeFile(virtual_page[i + VIRTUAL_NUM - PAGE_NUM]);
        virtual_page[i + VIRTUAL_NUM - PAGE_NUM] =-1;
        virtual_page[page_index] = -1;
        
      }
    }
  }
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

        phys_page[i] = -1;
      }
    }
    int new_count = 0;
    for(int i = 0; i < PAGE_NUM; i++){
      if (phys_page[i] >= 0){
        printf("i: %d\n", new_count);
        new_count +=1;
      }
      counter=new_count;
    }
    
 

  printf("counter: %d new_count: %d\n", counter, new_count);
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

void* pm_swap_helper(void *ptr, int index_swap, int value){
  if (ptr == NULL){
    return;
  }
  if (counter < 0){
    return;
  }
  // int page_index = (ptr - (void*)phys_mem) / PM_PAGE_SIZE;    find how many pages need to be freed  
 
  // int index_free = page_index; 
  // int free_page = time_page[index_free]; 
  for (int j = 0; j < PAGE_NUM; j++) {
      if (virtual_page[j + VIRTUAL_NUM-PAGE_NUM] < 0){
        virtual_page[j + VIRTUAL_NUM-PAGE_NUM] = index_swap;
        storeArray(index_swap,value);
        break;
      }
  }
  virtual_page[index_swap] = -4;
  time_page[0] = -1;
  for (int i = 0; i < PAGE_NUM - 1; i++){
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
  // counter -=1;
  phys_page[0]=-1;
  for (int i = 0; i < PAGE_NUM - 1; i++){
    int temp = phys_page[i];
    phys_page[i] = phys_page[i + 1];
    phys_page[i+1] = temp;
   
  }
  int new_count;
  for(int i = 0; i < PAGE_NUM; i++){
    if (phys_page[i] >= 0){
      new_count++;
    }
  }
  counter = new_count;

  printf("counter_swap: %d\n", counter);

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


//create a db file to write the data
// void create_db_file(int index_swap) {
//   FILE *fp;
//   if ((fp = fopen("swapfile.txt", "w+")) == NULL) {
//     printf("Cannot open file.\n");
//     exit(1);
//   }
//   else {
//     //write the index_swap into the file
//     fwrite(&index_swap, sizeof(int), 1, fp);
//     printf("Writing index_swap into the swapfile.bin.\n");
//   }
// }


//remove the index_swap if db file with the index_swap num in it
// void remove_index_swap(int index_swap) {
//   FILE *fp;
//   int index;
//   if ((fp = fopen("swapfile.txt", "r+")) == NULL) {
//     printf("Cannot open file.\n");
//     exit(1);
//   }
//   else {
//     while (fread(&index, sizeof(index), 1, fp) != 0) {
//       if (index == index_swap) {
//         //remove the index_swap from the file
//         fseek(fp, -1 * sizeof(index), SEEK_CUR);
//         memset(&index, 0, sizeof(index));
//         fwrite(&index, sizeof(index), 1, fp);
//         break;
//       }
//     }
//     printf("Removing the index_swap from the swapfile.bin.\n");
//   }
//   //remove the db file if it is empty
//   fseek(fp, 0, SEEK_END);
//   long size = ftell(fp);
//   if (size == 0) {
//     remove("swapfile.bin");
//     printf("Removing the swapfile.bin.\n");
//   }
//   fclose(fp);
// }


void storeArray(int index, int value){
    FILE *fp;
    char fileName[20];
    sprintf(fileName, "%d.txt", index);

    fp = fopen(fileName, "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%d", value);
    fclose(fp);
}

int removeFile(int index){
    char fileName[20];
    sprintf(fileName, "%d.txt", index);

    int result = remove(fileName);
    if (result == 0)
        return 1;
    else
        return 0;
}


int fetchContent(int index){
    FILE *fp;
    char fileName[20];
    sprintf(fileName, "%d.txt", index);

    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int value;
    fscanf(fp, "%d", &value);
    fclose(fp);
    return value;
}
int *access_page(int num) {
  pthread_mutex_lock(&pm_mutex);
  if (num <0 || num >= (VIRTUAL_NUM - PAGE_NUM)){
    return NULL;
  }
  printf("virtual_page[%d]:%d\n", num,virtual_page[num]);
  //search the virtual page
  if (virtual_page[num] == -1){
    return NULL;
  } else if (virtual_page[num] == -4){
    for (int i =0; i< PAGE_NUM; i++){
      if (virtual_page[i + VIRTUAL_NUM - PAGE_NUM] == num){
        int file_fetched = fetchContent(num);
        printf("value_fetched: %d\n", file_fetched);
        return file_fetched;
      }
    }
  } else {
    int index = virtual_page[num];
    printf("value_get: %d\n", phys_page[index]);
    return phys_page[index];
  }
  return NULL;
  pthread_mutex_lock(&pm_mutex);

}