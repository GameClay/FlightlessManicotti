#include "core/memory.h"
#include "core/memcpy.h"
#include <stdlib.h>

void* default_malloc(size_t size, size_t alignment);
void default_free(void* pointer);

// Assign default memory operatons
_gcaligned_malloc_fn_ptr gcheap_alloc = &default_malloc;
_gcaligned_free_fn_ptr gcheap_free = &default_free;
_gcmemrcpy_fn_ptr gcmicrorcpy = &dvmemcpy;

// Default malloc/free
void* default_malloc(size_t size, size_t align_size)
{
   char *ptr, *ptr2, *aligned_ptr;
   int align_mask = align_size - 1;

   ptr = (char*)malloc(size + align_size + sizeof(int));
   if(ptr == NULL) 
      return NULL;

   ptr2 = ptr + sizeof(int);
   aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));


   ptr2 = aligned_ptr - sizeof(int);
   *((int*)ptr2) = (int)(aligned_ptr - ptr);

   return aligned_ptr;
}

void default_free(void* pointer)
{
   int* ptr2 = (int*)pointer - 1;
   pointer -= *ptr2;
   free(pointer);
}