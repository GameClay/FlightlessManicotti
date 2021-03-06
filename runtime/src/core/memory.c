/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2012 GameClay LLC
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
#include <stdint.h>
#include <stdlib.h>
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/core/memory.h>

/* Extern the memory functions used by default */
#if defined(KL_USE_NEDMALLOC)
extern void* nedmalloc(size_t size);
extern void nedfree(void *mem);
extern void* nedmalloc2(size_t size, size_t alignment, unsigned flags);
extern void nedfree2(void *mem, unsigned flags);
#endif

#if defined(KL_USE_MICROALLOCATOR)
#  include "MicroAllocator.h"
#endif

/* Forward declare */
void* aligned_nedmalloc(size_t size, size_t alignment);
void aligned_nedfree(void* pointer);
void* default_malloc(size_t size);
void default_free(void* pointer);

/* Assign default memory operatons */
kl_malloc_fn_ptr kl_heap_alloc_ptr = &default_malloc;
kl_free_fn_ptr kl_heap_free_ptr = &default_free;

kl_aligned_malloc_fn_ptr kl_heap_aligned_alloc_ptr = &aligned_nedmalloc;
kl_aligned_free_fn_ptr kl_heap_aligned_free_ptr = &aligned_nedfree;

#if defined(KL_USE_MICROALLOCATOR)
kl_malloc_fn_ptr kl_micro_alloc_ptr = &micro_malloc;
kl_free_fn_ptr kl_micro_free_ptr = &micro_free;
#else
kl_malloc_fn_ptr kl_micro_alloc_ptr = &default_malloc;
kl_free_fn_ptr kl_micro_free_ptr = &default_free;
#endif

kl_memrcpy_fn_ptr kl_microrcpy_ptr = &memcpy;

/* Implementations */
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
   KL_ASSERT(size <= 256, "Can not allocate more than 256 bytes with kl_micro_alloc.");
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

void* kl_microrcpy(void* KL_RESTRICT dest, const void* KL_RESTRICT src, size_t size)
{
   return kl_microrcpy_ptr(dest, src, size);
}

/* Default aligned malloc/free using nedmalloc */
void* default_malloc(size_t size)
{
#if defined(KL_OS_ANDROID)
   void* ret = memalign(16, size);
#elif defined(KL_VALGRIND) || !defined(KL_USE_NEDMALLOC)
   void* ret = NULL;
   posix_memalign(&ret, 16, size);
#else
   void* ret = nedmalloc2(size, 16, 0);
#endif
   KL_ASSERT((((uintptr_t)ret) & 0xF) == 0, "Return was not aligned!");
   return ret;
}

void default_free(void* p)
{
#if defined(KL_OS_ANDROID) || defined(KL_VALGRIND) || !defined(KL_USE_NEDMALLOC)
   free(p);
#else
   nedfree2(p, 0);
#endif
}

void* aligned_nedmalloc(size_t size, size_t align_size)
{
#if defined(KL_OS_ANDROID)
   KL_ASSERT(align_size != 16, "Use kl_malloc for 16-byte aligned allocations.");
   return memalign(align_size, size);
#elif defined(KL_VALGRIND) || !defined(KL_USE_NEDMALLOC)
   void* ret = NULL;
   KL_ASSERT(align_size != 16, "Use kl_malloc for 16-byte aligned allocations.");
   posix_memalign(&ret, align_size, size);
   return ret;
#else
   KL_ASSERT(align_size != 16, "Use kl_malloc for 16-byte aligned allocations.");
   return nedmalloc2(size, align_size, 0); /* No flags */
#endif
}

void aligned_nedfree(void* pointer)
{
#if defined(KL_OS_ANDROID) || defined(KL_VALGRIND) || !defined(KL_USE_NEDMALLOC)
   free(pointer);
#else
   nedfree2(pointer, 0); /* No flags */
#endif
}

void kl_zero_mem(void* dest, size_t size)
{
   memset(dest, 0, size);
}
