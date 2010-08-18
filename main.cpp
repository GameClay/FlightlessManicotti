#include <stdio.h>

int main(int argc, const char* argv[])
{
   printf("FlightlessManicotti\n");
   
   for(int i = 0; i < argc; i++)
   {
      printf("\targ %d: %s\n", i, argv[i]);
   }
   
   return 0;
}