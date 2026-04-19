#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "uintarr.h"

/*
Generates an array of a hard-coded size of uints, fills it in parallel with
either sorted or random numbers, and calculates its sumation in parallel and in
series
*/
int main(void) {

  // Array generation
  unsigned int size = -1; // Largest unsigned integer
  unsigned int *array = new_array(size, ARRAY_SORTED);

  // Verify array was generated successfully
  if (!array) {
    printf("Error while generating array\n");
    return 0;
  } else {
    printf("Array of %u elements generated\n", size);

    // For debugging with small arrays
    if (size <= 100) {
      print_array(array, size);
    }
  }

  // Parallel sum calculation and printout
  unsigned long total_parallel_sum = parallel_sum(array, size);
  printf("Array parallel sum:   %lu\n", total_parallel_sum);

  // Sequential sum calculation and printout
  unsigned long total_sequential_sum = sequential_sum(array, size);
  printf("Array sequential sum: %lu\n", total_sequential_sum);

  free(array);
  return 0;
}
