#include <stdio.h>
#include <stdlib.h>
#include "core/queue.h"
#include <assert.h>

#define NUM_TEST 1000000

int main(int argc, const char* argv[])
{
   printf("FlightlessManicotti\n");
   
   for(int i = 0; i < argc; i++)
   {
      printf("\targ %d: %s\n", i, argv[i]);
   }
   
   gcqueue test_queue;
   gcinit_queue(&test_queue, sizeof(int), NUM_TEST + 1, gcheap_alloc);
   
   // Build test set
   int test_set[NUM_TEST];
   for(int i = 0; i < NUM_TEST; i++)
   {
      test_set[i] = rand();
   }
   
   // Enqueue
   for(int i = 0; i < NUM_TEST; i++)
   {
      while(gcenqueue(&test_queue, &test_set[i]) < 0)
         ;
   }
   
   // Dequeue and verify
   int test_i;
   for(int i = 0; i < NUM_TEST; i++)
   {
      while(gcdequeue(&test_queue, &test_i) < 0)
         ;
      assert(test_set[i] == test_i);
   }
   
   gcdestroy_queue(&test_queue, gcheap_free);
   
   return 0;
}