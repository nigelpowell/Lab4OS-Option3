#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 5
#define THREAD_MAX 2

int array[SIZE];
int opt_a;
struct threadControl
{
    int threadCon_low;
    int threadCon_high;
    int threadCon_no;
};

void fillArrayWithRandomNumbers(int arr[SIZE]) 
{
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) 
{
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

void merger(int low, int middle, int high) 
{  
    int size1 = middle - low + 1;
    int size2 = high - middle, i, j; 
        
    int* left = malloc(size1 * sizeof(int)); 
    int* right = malloc(size2 * sizeof(int));
   
    for (i = 0; i < size1; i++) 
        left[i] = array[i + low]; 
    
    for (i = 0; i < size2; i++) 
        right[i] = array[i + middle + 1]; 
  
    int k = low; 
    i = j = 0; 
  
    while (i < size1 && j < size2) 
    { 
        if (left[i] <= right[j]) 
            array[k++] = left[i++]; 
        else
            array[k++] = right[j++]; 
    } 
    while (i < size1) 
    { 
        array[k++] = left[i++]; 
    }  
    while (j < size2) 
    { 
        array[k++] = right[j++]; 
    } 
    
    free(left);
    free(right);
} 

void mergeSortAlg(int low, int high) 
{  
    int middle = low + (high - low) / 2; 
    if (low < high) 
    {  
        mergeSortAlg(low, middle);
        mergeSortAlg(middle + 1, high);
        merger(low, middle, high); 
    } 
}

void* mergeSort(void* args) 
{
    
    struct threadControl *threadControl = args;
    int low = threadControl->threadCon_low;
    int high = threadControl->threadCon_high; 
    int middle = low + (high - low) / 2;
    if (low < high) 
    { 
        mergeSortAlg(low, middle); 
        mergeSortAlg(middle + 1, high); 
        merger(low, middle, high); 
    }
}

int main() 
{
    srand(time(0));
    struct threadControl *threadControl;
    opt_a = 1;
    fillArrayWithRandomNumbers(array);    
    printf("Unsorted array: ");
    printArray(array);
    pthread_t threads[THREAD_MAX]; 
    struct threadControl threadConList[THREAD_MAX];
    int len = SIZE / THREAD_MAX;
    int low = 0;
    
    for (int i = 0; i < THREAD_MAX; i++, low += len) 
    {
        threadControl = &threadConList[i];
        threadControl->threadCon_no = i;
        if (opt_a) 
        {
            threadControl->threadCon_low = low;
            threadControl->threadCon_high = low + len - 1;
            if (i == (THREAD_MAX - 1))
                threadControl->threadCon_high = SIZE - 1;
        }
        else 
        {
            threadControl->threadCon_low = i * (SIZE / THREAD_MAX);
            threadControl->threadCon_high = (i + 1) * (SIZE / THREAD_MAX) - 1;
        }
    }
    
    for (int i = 0; i < THREAD_MAX; i++)
    { 
        threadControl = &threadConList[i];
        pthread_create(&threads[i], NULL, mergeSort, threadControl); 
    } 

    for (int i = 0; i < THREAD_MAX; i++)
    {
        pthread_join(threads[i], NULL);
    } 
    
    if (opt_a) 
    {
        struct threadControl *tcb = &threadConList[0];
        for (int i = 1; i < THREAD_MAX; i++) 
        {
            struct threadControl *threadControl = &threadConList[i];
            merger(tcb->threadCon_low, threadControl->threadCon_low - 1, threadControl->threadCon_high);
        }
    }
    else 
    {
        merger(0, (SIZE / 2 - 1) / 2, SIZE / 2 - 1);
        merger(SIZE / 2, SIZE / 2 + (SIZE - 1 - SIZE / 2) / 2, SIZE - 1);
        merger(0, (SIZE - 1) / 2, SIZE - 1);
    } 
    printf("Sorted array:   ");
    printArray(array);
}
