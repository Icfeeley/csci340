#include <stdio.h>    /* for printf statements when debugging */
#include <stdlib.h>   /* for malloc() and free() */
#include "mem.h"

/*
  Physical memory array. This is a static global array for all
  functions in this file.  An element in the array with a value of
  zero represents a free unit of memory.
*/
static dur_t* memory;

/*
 The size (i.e. number of units) of the physical memory array. This is
 a static global variable used by functions in this file.
 */
static int mem_size;

/*
 The last_placement_position variable contains the end position of the
 last allocated unit used by the next fit placement algorithm.
 */
static int last_placement_position = 0;

/*
  Using the memory placement algorithm, strategy, allocate size
  units of memory that will reside in memory for duration time units.

  If successful, this function returns the number of contiguous blocks
  (a block is a contiguous "chuck" of units) of free memory probed while
  searching for a suitable block of memory according to the placement
  strategy specified.  If unsuccessful, return -1.

  If a suitable contiguous block of memory is found, the first size
  units of this block must be set to the value, duration.
 */
int mem_allocate(mem_strats_t strategy, int size, dur_t duration)
{
  /*
  FIRSTFIT
  */
  if (strategy == FIRSTFIT) {
    int i;
    int num_probes = 0;
    int free_block_count = 0;
    int free_block_start = -1;
    for (i = 0; i < mem_size; i++){
      if (free_block_count >= size) {
        for (i = 0; i < size; i++) {
          if (free_block_start + i < mem_size) {
            memory[(free_block_start +i)] = duration;
          }
        }
        return num_probes;
      }

      if (memory[i] == 0) {
        if (free_block_start == -1) {
          free_block_start = i;
          num_probes++;
        }
        free_block_count++;
      }
      else if (memory[i] != 0) {
        free_block_start = -1;
        free_block_count = 0;
      }
    }
    // No spot large enough in memory to allocate request
    return -1;
  }

  /*
  NEXTFIT
  */
  if (strategy == NEXTFIT) {
    int i;
    int num_probes = 0;
    int free_block_count = 0;
    int free_block_start = -1;
    int offset = mem_size - last_placement_position;
    for (i = 0; i < offset; i++){

      if (free_block_count >= size) {
        for (i = 0; i < size; i++) {
          if (free_block_start+i+last_placement_position <mem_size) {
            memory[(free_block_start +i + last_placement_position)] = duration;
          }
        }
        last_placement_position = free_block_start + i;
        return num_probes;
      }

      if (memory[i + last_placement_position] == 0) {
        if (free_block_start == -1) {
          free_block_start = i + last_placement_position;
          num_probes++;
        }
        free_block_count++;
      }
      else if (memory[i + last_placement_position] != 0) {
        free_block_start = -1;
        free_block_count = 0;
      }
    }

    free_block_count = 0;
    free_block_start = -1;
    for (i = 0; i < last_placement_position; i++){
      // printf("%d < %d\n",i, last_placement_position);
      if (free_block_count >= size) {
        for (i = 0; i < size; i++) {
          if (free_block_start + i < mem_size) {
            /* code */
            memory[(free_block_start +i)] = duration;
          }
          last_placement_position = free_block_start + i;
        }
        return num_probes;
      }

      if (memory[i] == 0) {
        if (free_block_start == -1) {
          free_block_start = i;
          num_probes++;
        }
        free_block_count++;
      }
      else if (memory[i] != 0) {
        free_block_start = -1;
        free_block_count = 0;
      }
    }
    // No spot large enough in memory to allocate request
    return -1;
  }

  /*
  BESTFIT
  */
  if (strategy == BESTFIT) {
    int i;
    int num_probes = 0;
    int free_block_count = 0;
    int free_block_start = -1;
    int best_block_start = -1;
    int best_block_count = mem_size;
    int diff;
    int best_diff = mem_size;
    for (i = 0; i < mem_size; i++){
      if (memory[i] == 0) {
        if (free_block_start == -1) {
          free_block_start = i;
          num_probes++;
        }
        free_block_count++;
      }
      else if (memory[i] != 0) {
        if (free_block_count >= size) {
          diff = free_block_count - size;
          if(diff < best_diff){
            best_diff = free_block_count - size;
            best_block_count = free_block_count;
            best_block_start = free_block_start;
          }
        }
        free_block_start = -1;
        free_block_count = 0;
      }
    }
    if (free_block_count >= size && best_block_start == -1) {
      for (i = 0; i < size; i++) {
        if(free_block_start + i < mem_size){
          memory[(free_block_start +i)] = duration;
        }
        // printf("memory[free_block_start + %d] = %d\n",i, memory[(free_block_start +i)]);
      }
      return num_probes;
    }
    else if (best_block_start != -1) {
      for (i = 0; i < size; i++) {
        if (best_block_start + i < mem_size ){
          memory[(best_block_start+i)] = duration;
        }
        // printf("!!!memory[best_block_start+%d] = %d\n",i, memory[best_block_start+i] );
      }
      return num_probes;
    }
    else {
      // printf("fail\n");
      // No spot large enough in memory to allocate request
      return -1;
    }
  }
  return -2;
}

/*
  Go through all of memory and decrement all positive-valued entries.
  This simulates one unit of time having transpired.  NOTE: when a
  memory cell is decremented to zero, it becomes "unallocated".
 */
int mem_single_time_unit_transpired()
{
  int i;
  for (i = 0; i < mem_size; i ++){
    if (memory[i] > 0) {
      memory[i]--;
    }
  }
  return 1;
}

/*
  Return the number of fragments in memory.  A fragment is a
  contiguous free block of memory of size less than or equal to
  frag_size.
 */
int mem_fragment_count(int frag_size)
{
  int i;
  int free_block_count = 0;
  int frag_count = 0;
  for (i = 0; i < mem_size; i ++){
    if (memory[i] > 0) {
      if (free_block_count > 0 && free_block_count <= frag_size) {
        frag_count++;
      }
      free_block_count = 0;
    }
    else{
      free_block_count++;
    }

  }

  return frag_count;
}

/*
  Set the value of zero to all entries of memory.
 */
void mem_clear()
{
  int i;
  for (i = 0; i < mem_size; i ++){
    memory[i] = 0;
  }
}

/*
 Allocate physical memory to size. This function should
 only be called once near the beginning of your main function.
 */
void mem_init(int size)
{
  memory = malloc(sizeof(dur_t)*size);
  mem_size = size;
}

/*
 Deallocate physical memory. This function should
 only be called once near the end of your main function.
 */
void mem_free()
{
  free(memory);
}

/*
  Print memory for testing/debugging purposes.  This is an optional
  routine to write, but highly recommended!  You will need to test and
  debug your allocation algorithms.  Calling this routine in your
  main() after every allocation (successful or not) will help in this
  endeavor.  NOTE: you should print the memory contents in contiguous
  blocks, rather than single units; otherwise, the output will be very
  long.
 */
void mem_print()
{
}

int rand_range(int min, int max){
  int diff = max - min;
  return (int) (((double)(diff+1)/RAND_MAX) * rand() + min);
}
