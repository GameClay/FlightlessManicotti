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
 
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "fm.h"
#include "core/memory.h"

// Extern the memory functions used by default
extern void* nedmalloc(size_t size);
extern void nedfree(void *mem);
extern void* nedmalloc2(size_t size, size_t alignment, unsigned flags);
extern void nedfree2(void *mem, unsigned flags);

#include "MicroAllocator.h"

// Forward declare
void* aligned_nedmalloc(size_t size, size_t alignment);
void aligned_nedfree(void* pointer);

// Assign default memory operatons
kl_malloc_fn_ptr kl_heap_alloc_ptr = &nedmalloc;
kl_free_fn_ptr kl_heap_free_ptr = &nedfree;

kl_aligned_malloc_fn_ptr kl_heap_aligned_alloc_ptr = &aligned_nedmalloc;
kl_aligned_free_fn_ptr kl_heap_aligned_free_ptr = &aligned_nedfree;

kl_malloc_fn_ptr kl_micro_alloc_ptr = &micro_malloc;
kl_free_fn_ptr kl_micro_free_ptr = &micro_free;

kl_memrcpy_fn_ptr kl_microrcpy_ptr = &memcpy;

// Implementations
void* kl_heap_alloc(size_t size)
{
   return kl_heap_alloc_ptr(size);
}

void kl_heap_free(void* pointer)
{
   kl_heap_free_ptr(pointer);
}

void* kl_micro_alloc(size_t size)
{
   return kl_micro_alloc_ptr(size);
}

void kl_micro_free(void* pointer)
{
   kl_micro_free_ptr(pointer);
}

void* kl_heap_aligned_alloc(size_t size, size_t align_size)
{
   return kl_heap_aligned_alloc_ptr(size, align_size);
}

void kl_heap_aligned_free(void* pointer)
{
   kl_heap_aligned_free_ptr(pointer);
}

void* kl_microrcpy(void* restrict dest, const void* restrict src, size_t size)
{
   return kl_microrcpy_ptr(dest, src, size);
}

// Default aligned malloc/free using nedmalloc
void* aligned_nedmalloc(size_t size, size_t align_size)
{
   assert(align_size != 16); 
   return nedmalloc2(size, align_size, 0); // No flags
}

void aligned_nedfree(void* pointer)
{
   nedfree2(pointer, 0); // No flags
}