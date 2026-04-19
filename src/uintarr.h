// For specifying the type of values that will fill a newly generated array
enum filling_modes { ARRAY_SORTED, ARRAY_RANDOM, NUM_MODES };

// unsigned int max_array_size();
unsigned int *new_array(unsigned int size, unsigned int population_mode);
void print_array(unsigned int *array, unsigned int size);
unsigned long parallel_sum(unsigned int *array, unsigned int size);
unsigned long sequential_sum(unsigned int *array, unsigned int size);