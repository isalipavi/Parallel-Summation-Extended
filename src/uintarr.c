#include "uintarr.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// For including subarray data in other structs of thread-specific data
typedef struct {
  unsigned int *array;
  unsigned int start;
  unsigned int end;
} sub_data_t;

// For passing thread-specific data to the fill_array function
typedef struct {
  sub_data_t sub_data;
  unsigned int mode;
} fill_array_data_t;

// For passing thread-specific data to the parallel_sum function
typedef struct {
  sub_data_t sub_data;
  unsigned long *total_sum;
  pthread_mutex_t *mutex;
} parallel_sum_data_t;

sub_data_t set_sub_data(unsigned int *array, unsigned int size, int t_num,
                        int t_i);
void *fill_array(void *args);
void *thread_sum(void *args);

// Calculates the maximum number of integers the current system can store
// unsigned int max_array_size() {
//   unsigned long available_pages = sysconf(_SC_AVPHYS_PAGES);
//   unsigned long page_size = sysconf(_SC_PAGESIZE);

//   unsigned long available_bytes = available_pages * page_size;
//   unsigned int available_ints = available_bytes / sizeof(int);

//   return available_ints;
// }

// Generates a dynamic array of unsigned integers of a given size
// It will be filled with either sorted (from 1 to size) or random numbers
unsigned int *new_array(unsigned int size, unsigned int population_mode) {

  // Verify population mode validity
  if (population_mode == ARRAY_RANDOM) {
    srand(time(NULL));
  } else if (population_mode >= NUM_MODES) {
    return NULL;
  }

  // New array of unsigned integers
  unsigned int *array = malloc(size * sizeof(int));

  // Number of threads and their IDs
  int t_num = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_t t_ids[t_num];

  // Start array population in parallel
  for (int t_i = 0; t_i < t_num; ++t_i) {
    fill_array_data_t *t_data = malloc(sizeof(fill_array_data_t));
    t_data->sub_data = set_sub_data(array, size, t_num, t_i);
    t_data->mode = population_mode;

    pthread_create(&t_ids[t_i], NULL, fill_array, t_data);
  }

  // Join threads
  for (int t_i = 0; t_i < t_num; ++t_i) {
    pthread_join(t_ids[t_i], NULL);
  }

  return array;
}

// Prints an array of unsigned integers of a given size
void print_array(unsigned int *array, unsigned int size) {
  for (unsigned int i = 0; i < size; ++i) {
    printf("%u ", array[i]);
  }
}

// Calculates the sum of the elements of an array in parallel
unsigned long parallel_sum(unsigned int *array, unsigned int size) {
  unsigned long total_sum = 0; // Summation result

  // Mutual exclusion for performing summation in parallel
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);

  // Number of threads and their IDs
  int t_num = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_t t_ids[t_num];

  // Start array summation in parallel
  for (int t_i = 0; t_i < t_num; ++t_i) {

    // Set required data for current thread
    parallel_sum_data_t *t_data = malloc(sizeof(parallel_sum_data_t));
    t_data->sub_data = set_sub_data(array, size, t_num, t_i);
    t_data->total_sum = &total_sum;
    t_data->mutex = &mutex;

    // Perform summation
    pthread_create(&t_ids[t_i], NULL, thread_sum, t_data);
  }

  // Destoy mutual exclusion
  pthread_mutex_destroy(&mutex);

  // Join threads
  for (int t_i = 0; t_i < t_num; ++t_i) {
    pthread_join(t_ids[t_i], NULL);
  }

  return total_sum;
}

// Calculate the sum of the elements of an array in sequence/series
unsigned long sequential_sum(unsigned int *array, unsigned int size) {
  unsigned long total_sum = 0; // Summation result

  // Perform the summation element by element
  for (unsigned int i = 0; i < size; ++i) {
    total_sum += array[i];
  }

  return total_sum;
}

// Calculate the data needed for a thread to perform a task over a subarray
sub_data_t set_sub_data(unsigned int *array, unsigned int size, int t_num,
                        int t_i) {
  sub_data_t data = {0}; // For storing the data

  unsigned int sub_size = size / t_num; // Base size of the subarray
  int remainder = size % t_num; // Number of unaccounted elements in array
  int offset = t_i < remainder ? t_i : remainder; // For offsetting start index

  // Initialize data
  data.array = array;
  data.start = sub_size * t_i + offset;
  data.end = data.start + sub_size + (t_i < remainder);

  return data;
}

// Fills a subarray with sorted or random numbers
void *fill_array(void *args) {
  fill_array_data_t *data = (fill_array_data_t *)args; // Retrieve data

  // Fills the array according to the filling mode indicated in data
  switch (data->mode) {
  case ARRAY_SORTED:
    for (unsigned int i = data->sub_data.start; i < data->sub_data.end; ++i) {
      data->sub_data.array[i] = i + 1; // Use the current index
    }
    break;
  case ARRAY_RANDOM:
    for (unsigned int i = data->sub_data.start; i < data->sub_data.end; ++i) {
      data->sub_data.array[i] = rand() + 1; // Use a random number
    }
    break;
  }

  free(data);
  pthread_exit(NULL);
  return NULL;
}

// Sums the elements of a subarray into a local variable
// This local sum is then added to a global sum variable protected by a mutex
void *thread_sum(void *args) {
  parallel_sum_data_t *data = (parallel_sum_data_t *)args; // Retrieve data
  unsigned long local_sum = 0; // Result of the local summation

  // Perform the summation of the elements of the subarray
  for (unsigned int i = data->sub_data.start; i < data->sub_data.end; ++i) {
    local_sum += data->sub_data.array[i];
  }

  // Add the local result to the global one, protected by a mutex
  pthread_mutex_lock(data->mutex);
  *data->total_sum += local_sum;
  pthread_mutex_unlock(data->mutex);

  free(data);
  pthread_exit(NULL);
  return NULL;
}