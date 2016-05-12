/*********************************************************************
 *
 *  This file is part of Sort.
 * 
 *  Tested on gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.1) 
 *
 *  Sort was developed by Angelo Mendoza and Zachary Martin.
 *
 *  Sort is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Sort is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Sort.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/****************************************************************************
 * THREADED SORT FUNCTION ARGUMENTS STRUCT
 * DESCRIPTION: Holds the arguments that are passed to the threaded 
 *     quick sort and threaded merge sort algorithms
 */
struct sort_args {
    int *array;     // Array to be sorted
    unsigned low;   // Lower bound
    unsigned high;  // Upper bound
};

/**************************************************************************
 **************************************************************************
 *    _____ _   _ _____ _____  _   __  _____  ___________ _____ 
 *   |  _  | | | |_   _/  __ \| | / / /  ___||  _  | ___ \_   _|
 *   | | | | | | | | | | /  \/| |/ /  \ `--. | | | | |_/ / | |  
 *   | | | | | | | | | | |    |    \   `--. \| | | |    /  | |  
 *   \ \/' / |_| |_| |_| \__/\| |\  \ /\__/ /\ \_/ / |\ \  | |  
 *    \_/\_\\___/ \___/ \____/\_| \_/ \____/  \___/\_| \_| \_/  
 *
 */

/**************************************************************************
 * QUICK SORT SWAP FUNCTION
 * DESCRIPTION: Swaps two integers
 * PASSED TO THIS FUNCTION: 
 *     int *a: The first integer
 *     int *b: The second integer
 */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    return;
}

/****************************************************************************
 * QUICK SORT HOAR PARTITION FUNCTION
 * DESCRIPTION: Splits the array into two sub-sections with a pivot
 * PASSED TO THIS FUNCTION: 
 *    int *array: The array to be sorted
 *    unsigned low:    The first index of the array
 *    unsigned high:   The last index of the array 
 */
int partition(int *array, unsigned low, unsigned high) {
  int pivot_value = array[(low + high)/2];
  int temp;
  unsigned i = low - 1;
  unsigned j = high + 1;
  
  while(1) {
    
    do {
      i++;
    }  while(array[i] < pivot_value);
    
    do {
      j--;
    } while(array[j] > pivot_value);
    
    if(i < j) {
      swap(&array[i], &array[j]);
    } else {
      return j;
    }
  }
}

/****************************************************************************
 * QUICK SORT WORKER FUNCTION
 * DESCRIPTION: Creates a thread for each child created in the quicksortma
 * PASSED TO THIS FUNCTION: 
 *    *args: pointer to sort_args struct containing the arguments for the sort
 */
void *quick_sort_worker(void *args) {
  pthread_t tid[2];
  struct sort_args child_args[2];
  struct sort_args *root_args = args;
  
  int *array = root_args->array;
  unsigned low = root_args->low;
  unsigned high = root_args->high;
  unsigned pivot;
  int i;
  
  pthread_attr_t attr;
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
    if(low < high) {
        pivot = partition(array, low, high);
        child_args[0].array = array;
        child_args[0].low = low;
        child_args[0].high = pivot;
  
        child_args[1].array = array;
        child_args[1].low = pivot + 1;
        child_args[1].high = high;
 
        pthread_create(&tid[0], &attr, quick_sort_worker, (void *) &child_args[0]);
        pthread_create(&tid[1], &attr, quick_sort_worker, (void *) &child_args[1]);

        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
        
    }
    
  pthread_exit(0);
}

/****************************************************************************
 * SINGLE THREADED QUICK SORT START FUNCTION
 * DESCRIPTION: This function starts the single-threaded quicksort algorithm
 * PASSED TO THIS FUNCTION: 
 *    int *array: The array to be sorted
 *    unsigned low:    The first index of the array
 *    unsigned high:   The last index of the array 
 */
void single_threaded_quick_sort(int *array, unsigned low, unsigned high) {
    unsigned pivot;
    if(low < high) {
        pivot = partition(array, low, high);
        single_threaded_quick_sort(array, low, pivot);
        single_threaded_quick_sort(array, pivot + 1, high);
    }
    return;
}

/****************************************************************************
 * MULTI THREADED QUICK SORT START FUNCTION
 * DESCRIPTION: This function starts the multi-threaded quicksort algorithm
 * PASSED TO THIS FUNCTION: 
 *    int *array: The array to be sorted
 *    unsigned low:    The first index of the array
 *    unsigned high:   The last index of the array 
 */
void multi_threaded_quick_sort(int *array, unsigned low, unsigned high) {

  struct sort_args args;
  args.array = array;
  args.low = low;
  args.high = high;
  
  pthread_t tid;
    
  // Set attributes
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
  // Create thread
  pthread_create(&tid, &attr, quick_sort_worker, (void *) &args);
  pthread_join(tid, NULL);
}


/**************************************************************************
 **************************************************************************
 *   ___  ___ ___________ _____  _____   _____  ___________ _____ 
 *   |  \/  ||  ___| ___ \  __ \|  ___| /  ___||  _  | ___ \_   _|
 *   | .  . || |__ | |_/ / |  \/| |__   \ `--. | | | | |_/ / | |  
 *   | |\/| ||  __||    /| | __ |  __|   `--. \| | | |    /  | |  
 *   | |  | || |___| |\ \| |_\ \| |___  /\__/ /\ \_/ / |\ \  | |  
 *   \_|  |_/\____/\_| \_|\____/\____/  \____/  \___/\_| \_| \_/ 
 *
 */
 
/****************************************************************************
 * MERGE SORT MERGE FUNCTION
 * DESCRIPTION: Merges two array slices into one.
 * PASSED TO THIS FUNCTION:
 *    int *array: The array to be sorted
 *    unsigned low:      Lower boundary of the array slice
 *    unsigned midpoint: Midpoint of the array slice
 *    unsigned high:     Upper boundary of the array slice */
void merge(int array[], unsigned low, unsigned midpoint, unsigned high) {
    int temp[high-low+1];    // Temporary array
    unsigned i = low;        
    unsigned j = high;       
    unsigned m = midpoint+1; 
    unsigned k = 0;          // temp's array index
    
    while (i <= midpoint && m <= j) {
        if (array[i] <= array[m]){
            temp[k++] = array[i++];
        } else {
            temp[k++] = array[m++];
        }
    }
    
    while (i <= midpoint) {
        temp[k++] = array[i++];
    }
    
    while (m <= j) {
        temp[k++] = array[m++];
    }
    
    // Reset i and j counters
    i = low;
    j = 0;
    
    // Copy contents of the temporary array back into the original array
    while (i <= high) {
        array[i++] = temp[j++];
    }
}

/****************************************************************************
 * MULTI-THREADED MERGE SORT WORKER FUNCTION
 * DESCRIPTION: Creates a thread for each child created in the merge sort
 * PASSED TO THIS FUNCTION:
 *    *args: pointer to sort_args struct containing the arguments for the sort
 */
void *merge_sort_worker(void *args) {
    pthread_t tid[2];
    struct sort_args child_args[2];
    struct sort_args *root_args = args;
    
    int *array = root_args->array;
    unsigned low = root_args->low;
    unsigned high = root_args->high;
    unsigned midpoint;
    unsigned i;
    
    // Set pthread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    if (low < high){
        midpoint = (low + high) / 2;

        child_args[0].array = array;
        child_args[0].low = low;
        child_args[0].high = midpoint;
        child_args[1].array = array;
        child_args[1].low = midpoint+1;
        child_args[1].high = high;
                
        // Create threads
        pthread_create(&tid[0], &attr, &merge_sort_worker, (void *) &child_args[0]);
        pthread_create(&tid[1], &attr, &merge_sort_worker, (void *) &child_args[1]);
        
        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
    

        merge(array, low, midpoint, high);
        
        pthread_exit(NULL);
    }
}

/****************************************************************************
 * SINGLE-THREADED MERGE SORT START FUNCTION
 * DESCRIPTION: This function starts the single-threaded merge sort algorithm
 * PASSED TO THIS FUNCTION:
 *    int *array: The array to be sorted
 *    unsigned low:    The first index of the array
 *    unsigned high:   The last index of the array 
 */
void single_threaded_merge_sort(int *array, unsigned low, unsigned high) {
    unsigned midpoint;
    
    if (low < high) {
        midpoint = (low+high)/2;
        single_threaded_merge_sort(array, low, midpoint);
        single_threaded_merge_sort(array, midpoint+1, high);          
        merge(array, low, midpoint, high); 
    }
}

/****************************************************************************
 * MULTI-THREADED MERGE SORT START FUNCTION
 * DESCRIPTION: This function starts the multi-threaded merge sort algorithm
 * PASSED TO THIS FUNCTION:
 *    int *array: The array to be sorted
 *    unsigned low:    The first index of the array
 *    unsigned high:   The last index of the array 
 */
void multi_threaded_merge_sort(int *array, unsigned low, unsigned high) {
    struct sort_args args;
    args.array = array;
    args.low = low;
    args.high = high;
    
    pthread_t tid;

    //Set thread attributes
    pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	//Create first thread
    pthread_create(&tid, &attr, &merge_sort_worker, (void *) &args);
    pthread_join(tid, NULL);
    
}
