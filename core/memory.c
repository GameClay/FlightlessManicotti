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
 
#include "core/memory.h"
#include "core/memcpy.h"
#include <stdlib.h>
#include <string.h>

void* default_malloc(size_t size, size_t alignment);
void default_free(void* pointer);

// Assign default memory operatons
_gcaligned_malloc_fn_ptr gcheap_alloc = &default_malloc;
_gcaligned_free_fn_ptr gcheap_free = &default_free;
_gcmemrcpy_fn_ptr gcmicrorcpy = &memcpy;

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