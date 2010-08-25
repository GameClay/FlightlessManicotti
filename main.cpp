/* FlightlessManicotti -- Copyright (C) 2009-2010 GameClay LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "core/queue.h"
#include <assert.h>
#include <threads.h>

#define NUM_TEST 1000000

int main(int argc, const char* argv[])
{
   printf("FlightlessManicotti\n");
   
   for(int i = 0; i < argc; i++)
   {
      printf("\targ %d: %s\n", i, argv[i]);
   }
   
   gcqueue test_queue;
   gcalloc_queue(&test_queue, sizeof(int), NUM_TEST + 1);
   
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
   
   gcfree_queue(&test_queue);
   
   return 0;
}