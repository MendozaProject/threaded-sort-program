/****************************************************************************
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
 ****************************************************************************/

#include "sort.h"

#include <argp.h>

#define MAX_SIZE 2147483644 // Maximum integer size minus 3 

/****************************************************************************
 * GNERAL PROGRAM INFORMATION
 */
const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "GNU.Sort@gmail.com";
static char doc[] = "This program sorts a list of integers using quick sort or merge sort with or without threading.";
static char args_doc[] = "-- [integer_1] [integer_2] ...";

/****************************************************************************
 * ARGUMENT PARSER OPTIONS STRUCT
 * DESCRIPTION: Holds the different options that can be defined on the 
 *    command line for running the program
 */
static struct argp_option options[] = {
    {"threaded",  't', 0, 0, "Multi-threaded" },
    {"mergesort", 'm', 0, 0, "Merge-Sort" },
    {"quicksort", 'q', 0, 0, "Quick-Sort" },
    { 0 }
};

/****************************************************************************
 * ARRAY STRUCT
 * DESCRIPTION: Holds a pointer to a dynamically sized integer array 
 *    as well as an integer to keeps track of the size of the array
 */
struct array {
    int *list;
    unsigned size;
};

/****************************************************************************
 * ARGUMENT PARSER ARGUMENTS STRUCT
 * DESCRIPTION: Holds enums that define the different options 
 *    that the program can run with as well as a struct that contains an 
 *    array that will hold the unsorted integers to be sorted.
 */
struct arguments {
    enum { mergesort, quicksort } sorting;
    enum { single_threaded, multi_threaded } threading;
    struct array args;
};

/****************************************************************************
 * ARGUMENT PARSER PARSING OPTIONS FUNCTION
 * DESCRIPTION: Contains a switch statement which parses each argument
 *    passed to the program and determines which options to run with as well as 
 *    what numbers are to be sorted. 
 * PASSED TO THIS FUNCTION: 
 *    int key:   key from argument read from the command line
 *    char *arg: pointer to the command line argument
 *    struct argp_state *state: The current state of the argument parser.
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key){
        case 'q':
            arguments->sorting = quicksort;
            break;
        case 'm':
            arguments->sorting = mergesort;
            break;
        case 't':
            arguments->threading = multi_threaded;
            break;

        case ARGP_KEY_ARG: // Non option argument
            if (state->arg_num > MAX_SIZE){ // Too many arguments.
                argp_usage (state);
            }
            // Appends the argument to the args.list array
            // and increments args.size
            arguments->args.list[state->arg_num] = atoi(arg);
            arguments->args.size++;
            break;
            
        case ARGP_KEY_END: // End of arguments list
            if (state->arg_num < 1){ //Not enough arguments.
                argp_usage (state);
            }
            break;
            
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/****************************************************************************
 * ARGUMENT PARSER STRUCT
 * DESCRIPTION: Struct used by the argp library 
 */
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0};


/****************************************************************************
 * PRINT ARRAY FUNCTION
 * DESCRIPTION: Prints the elements in a given array between the lower and
 *    upper bounds given.
 * PASSED TO THIS FUNCTION:
 *    int *array: The array to be printed
 *    int low:      Lower boundary of the array slice
 *    int high:     Upper boundary of the array slice 
 */
void print_array(int array[], unsigned low, unsigned high) {
    unsigned i;
    for (i = low; i <= high; i++)
        printf("%d ",array[i]);
}

/****************************************************************************
 * SORT MAIN FUNCTION
 * DESCRIPTION: Grabs arguments and options from the command line.
 *    Calls arp_parse to parse options and arguments which stores the result
 *    in an arguments struct. Using the information in the struct
 *    chooses which sorting method (quick sort and merge sort) and 
 *    whether or not to use threads. Prints the passed (unsorted) array 
 *    and the array after sorting. 
 */
int main(int argc, char **argv) {
    struct arguments arguments;
    
    unsigned i, length;

    // Default is single threaded quicksort
    arguments.sorting = quicksort;
    arguments.threading = single_threaded;
    
    // Allocate memory for the dynamic array
    arguments.args.list = malloc(sizeof(int) * (argc - 1));
    arguments.args.size = 0;
    
    // Parse arguments to the program.
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    
    // number of integers in the list to be sorted
    length = arguments.args.size;
    
    printf("\nUnsorted Array: [ ");
    print_array(arguments.args.list, 0, arguments.args.size-1);
    printf("]\n");
    
    // Uses the state of arguments struct to determine which method to take
    if (arguments.sorting == quicksort && arguments.threading == single_threaded) {
        printf("Single-Threaded Quick Sort\n");
        single_threaded_quick_sort(arguments.args.list, 0,  length - 1);
    } else if (arguments.sorting == mergesort && arguments.threading == single_threaded) {
        printf("Single-Threaded Merge Sort\n");
        single_threaded_merge_sort(arguments.args.list, 0, length - 1);
    } else if (arguments.sorting == quicksort){
        printf("Multi-Threaded Quick Sort\n");
        multi_threaded_quick_sort(arguments.args.list, 0, length - 1);
    } else {
        printf("Multi-Threaded Merge Sort\n");
        multi_threaded_merge_sort(arguments.args.list, 0, length - 1);
    }
    
    // Prints out the sorted array.
    printf("Sorted Array:   [ ");
    print_array(arguments.args.list, 0, arguments.args.size-1);
    printf("]\n\n");

    free(arguments.args.list);
  
    exit(EXIT_SUCCESS);
}
