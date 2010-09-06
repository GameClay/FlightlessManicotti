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
#include <stdbool.h>
#include <assert.h>
#include "nedmalloc.h"
#include "MicroAllocator.h"

void* aligned_nedmalloc(size_t size, size_t alignment);
void aligned_nedfree(void* pointer);

// Assign default memory operatons
gc_malloc_fn_ptr gc_heap_alloc = &nedmalloc;
gc_free_fn_ptr gc_heap_free = &nedfree;

gc_aligned_malloc_fn_ptr gc_heap_aligned_alloc = &aligned_nedmalloc;
gc_aligned_free_fn_ptr gc_heap_aligned_free = &aligned_nedfree;

gc_malloc_fn_ptr gc_micro_alloc = &micro_malloc;
gc_free_fn_ptr gc_micro_free = &micro_free;

gc_memrcpy_fn_ptr gc_microrcpy = &memcpy;

// Default aligned malloc/free using nedmalloc
void* aligned_nedmalloc(size_t size, size_t align_size)
{
   assert(align_size != 16); 
   char *ptr, *ptr2, *aligned_ptr;
   intptr_t align_mask = align_size - 1;

   ptr = (char*)nedmalloc(size + align_size + sizeof(intptr_t));
   if(ptr == NULL) 
      return NULL;

   ptr2 = ptr + sizeof(intptr_t);
   aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));


   ptr2 = aligned_ptr - sizeof(intptr_t);
   *((intptr_t*)ptr2) = (intptr_t)(aligned_ptr - ptr);

   return aligned_ptr;
}

void aligned_nedfree(void* pointer)
{
   intptr_t* ptr2 = (intptr_t*)pointer - 1;
   void* free_ptr = (char*)pointer - *ptr2;
   nedfree(free_ptr);
}