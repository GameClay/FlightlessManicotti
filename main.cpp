#include <stdio.h>
#include "core/memory.h"

int main(int argc, const char* argv[])
{
   printf("FlightlessManicotti\n");
   
   for(int i = 0; i < argc; i++)
   {
      printf("\targ %d: %s\n", i, argv[i]);
   }
   
   int* test = (int*)gcheap_alloc(sizeof(int) * 5, 16);
   
   gcheap_free(test);
   
   return 0;
}