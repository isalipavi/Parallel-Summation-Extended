# Parallel-Summation-Extended
Base: Function that calculates the summation of an integer array
Extension: Generation of array with either sorted or random numbers in parallel, and calculate the summation  both in parallel and in sequence.

The solution was implemented with the use of pthreads for parallelization
To run, download the source files, open a terminal on the top workspace folder and run the commands "make all" and "make run"
For systems with around 16 Gb of RAM or less, set the array size (hardcoded in main) to around 3 billion or less
