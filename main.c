#include <stdio.h>    /* for printf */
#include <stdlib.h>   /* for string to integer conversion, random numbers */
#include "mem.h"

/*
  The main program will accept four paramemters on the command line.
  The first parameter is the memory size.  The second parameter is the
  duration of the each simulation run.  The third parameter is the
  number of times to repeat each experiment (ie. number of runs). The
  fourth parameter is a random number seed. Here is an example command
  line:

  ./hw7 1000 3000 100 1235

  This means that your program should initialize physical memory to
  1,000 units, perform 100 runs with each run taking 3000 units of
  time, and the random number generator should be seeded (one time)
  with the value 1235.
*/

int main(int argc, char** argv)
{
  /*
  -Gather comand line args
    -getop and use -d flag for debugging
  -Random nuber generator(seed value)
  -mem itit (number units)
  -for i in number runs
    while i < number time units
      mem allocate (random number, strategy)
      mem_single_time_unit_transpired
      mem_fragment_count
    mem_clear
  -mem_free

  */
  int mem_size;
  int num_time_units;
  int num_simulations;
  int seed;
  int frag_size = MIN_REQUEST_SIZE -1;
  int i = 0;
  int j = 0;
  int frag_count;
  int num_probes;
  float average_probes;
  float average_failures;
  float average_frags;
  int total_frags;
  int total_probes;
  int total_failures;
  if (argc != 5) {
    printf("Invalid number of arguments: supplied %d, excpected 4.\n", argc - 1);
    return 1;
  }

  mem_size = atoi(argv[1]);
  num_time_units = atoi(argv[2]);
  num_simulations = atoi(argv[3]);
  seed = atoi(argv[4]);
  srand(seed);
  mem_init(mem_size);

  total_probes = 0;
  total_failures = 0;
  total_frags = 0;

  for (i = 0; i < num_simulations; i++){
    int allocate_request_size;
    dur_t allocation_duration;
    j = 0;
    while (j < num_time_units) {
      mem_single_time_unit_transpired();
      int allocate_request_size = rand_range(MIN_REQUEST_SIZE, MAX_REQUEST_SIZE);
      dur_t allocation_duration = (unsigned char) rand_range(MIN_DURATION, MAX_DURATION);
      num_probes = mem_allocate(FIRSTFIT, allocate_request_size, allocation_duration);
      if (num_probes == -1) {
        total_failures++;
      }
      else {
        total_probes += num_probes;
      }
      frag_count = mem_fragment_count(frag_size);
      total_frags += frag_count;
      j++;
    }
  }
  average_probes = total_probes / (float) num_simulations;
  average_failures = total_failures / (float) num_simulations;
  average_frags = total_frags / (float) num_simulations;
  printf("FIRSTFIT:\n");
  printf("Average number of frags: %f\n", average_frags);
  printf("Average number of probes: %f\n", average_probes);
  printf("Average number of failures: %f\n", average_failures);
  mem_clear();

  total_probes = 0;
  total_failures = 0;
  total_frags = 0;
  for (i = 0; i < num_simulations; i++){
    int allocate_request_size;
    dur_t allocation_duration;
    j = 0;
    while (j < num_time_units) {
      mem_single_time_unit_transpired();
      int allocate_request_size = rand_range(MIN_REQUEST_SIZE, MAX_REQUEST_SIZE);
      dur_t allocation_duration = (unsigned char) rand_range(MIN_DURATION, MAX_DURATION);
      num_probes = mem_allocate(NEXTFIT, allocate_request_size, allocation_duration);
      if (num_probes == -1) {
        total_failures++;
      }
      else {
        total_probes += num_probes;
      }
      frag_count = mem_fragment_count(frag_size);
      total_frags += frag_count;
      j++;
    }
  }
  average_probes = total_probes / (float) num_simulations;
  average_failures = total_failures / (float) num_simulations;
  average_frags = total_frags / (float) num_simulations;
  printf("NEXTFIT:\n");
  printf("Average number of frags: %f\n", average_frags);
  printf("Average number of probes: %f\n", average_probes);
  printf("Average number of failures: %f\n", average_failures);
  mem_clear();

  total_probes = 0;
  total_failures = 0;
  total_frags = 0;
  for (i = 0; i < num_simulations; i++){
    int allocate_request_size;
    dur_t allocation_duration;
    j = 0;
    while (j < num_time_units) {
      mem_single_time_unit_transpired();
      int allocate_request_size = rand_range(MIN_REQUEST_SIZE, MAX_REQUEST_SIZE);
      dur_t allocation_duration = (unsigned char) rand_range(MIN_DURATION, MAX_DURATION);
      num_probes = mem_allocate(BESTFIT, allocate_request_size, allocation_duration);
      mem_print();
      if (num_probes == -1) {
        total_failures++;
      }
      else {
        total_probes += num_probes;
      }
      frag_count = mem_fragment_count(frag_size);
      total_frags += frag_count;
      j++;
    }
  }
  average_probes = total_probes / (float) num_simulations;
  average_failures = total_failures / (float) num_simulations;
  average_frags = total_frags / (float) num_simulations;
  printf("BESTFIT:\n");
  printf("Average number of frags: %f\n", average_frags);
  printf("Average number of probes: %f\n", average_probes);
  printf("Average number of failures: %f\n", average_failures);

  mem_free();

  return 0;
}
