/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void* default_malloc(size_t size, size_t alignment);
void default_free(void* pointer);

// Assign default memory operatons
gc_aligned_malloc_fn_ptr gc_heap_alloc = &default_malloc;
gc_aligned_free_fn_ptr gc_heap_free = &default_free;
gc_memrcpy_fn_ptr gc_microrcpy = &memcpy;

// Default malloc/free
void* default_malloc(size_t size, size_t align_size)
{
   char *ptr, *ptr2, *aligned_ptr;
   intptr_t align_mask = align_size - 1;

   ptr = (char*)malloc(size + align_size + sizeof(intptr_t));
   if(ptr == NULL) 
      return NULL;

   ptr2 = ptr + sizeof(intptr_t);
   aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));


   ptr2 = aligned_ptr - sizeof(intptr_t);
   *((intptr_t*)ptr2) = (intptr_t)(aligned_ptr - ptr);

   return aligned_ptr;
}

void default_free(void* pointer)
{
   intptr_t* ptr2 = (intptr_t*)pointer - 1;
   (char*)pointer -= *ptr2;
   free(pointer);
}