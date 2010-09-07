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
 
#ifndef _GC_MEMORY_H_
#define _GC_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <stddef.h>

//! @cond
typedef void* (*gc_malloc_fn_ptr)(size_t size);
typedef void (*gc_free_fn_ptr)(void* pointer);
typedef void* (*gc_aligned_malloc_fn_ptr)(size_t size, size_t align_size);
typedef void (*gc_aligned_free_fn_ptr)(void* pointer);
typedef void* (*gc_memrcpy_fn_ptr)(void* restrict dest, const void* restrict src, size_t size);

GC_API extern gc_malloc_fn_ptr gc_heap_alloc_ptr;
GC_API extern gc_free_fn_ptr gc_heap_free_ptr;
GC_API extern gc_aligned_malloc_fn_ptr gc_heap_aligned_alloc_ptr;
GC_API extern gc_aligned_free_fn_ptr gc_heap_aligned_free_ptr;
GC_API extern gc_malloc_fn_ptr gc_micro_alloc_ptr;
GC_API extern gc_free_fn_ptr gc_micro_free_ptr;
GC_API extern gc_memrcpy_fn_ptr gc_microrcpy_ptr;
//! @endcond

//! @defgroup memory_allocation Memory allocation
//! Functions which control memory allocation.
//!
//! All of these functions inline to function pointer acess. You may
//! change allocation behavior by reassigning the values of the function
//! pointers to alternate allocators.
//! @{   

//! Malloc function.
//!
//! This aligns all returned memory to a 16-byte boundary.
//!
//! @note The amount of allocated memory may exceed the size specified.
//!
//! @param size The size of the allocation.
//! @return Pointer the block of allocated memory, aligned on a 16-byte boundary,
//!         or NULL if the allocation failed.
GC_INLINE void* gc_heap_alloc(size_t size)
{
   return gc_heap_alloc_ptr(size);
}

//! Free function.
//!
//! @param pointer Pointer to the block of memory to free.
GC_INLINE void gc_heap_free(void* pointer)
{
   gc_heap_free_ptr(pointer);
}

//! Aligned malloc function.
//!
//! @note The amount of allocated memory may exceed the size specified.
//!
//! @attention This function will assert if the align_size == 16. If you want alignment
//!            on a 16-byte boundary use gc_heap_malloc, which returns 16-byte aligned
//!            memory at all times.
//!
//! @param size The size of the allocation.
//! @param align_size The boundary on which the allocated memory should be aligned.
//! @return Pointer the block of allocated memory, aligned on the specified boundary,
//!         or NULL if the allocation failed.
GC_INLINE void* gc_heap_aligned_alloc(size_t size, size_t align_size)
{
   return gc_heap_aligned_alloc_ptr(size, align_size);
}

//! Aligned free function.
//!
//! @param pointer Pointer to the block of memory to free.
GC_INLINE void gc_heap_aligned_free(void* pointer)
{
   gc_heap_aligned_free_ptr(pointer);
}

//! Micro malloc function.
//!
//! Allocates small amounts of memory 
//!
//! @attention If the allocation size is greater than 256 bytes, this
//!            function will assert, and return NULL if NDEBUG is defined.
//!
//! @param size The size of the allocation.
//! @return Pointer the block of allocated memory or NULL if the allocation failed.     
GC_INLINE void* gc_micro_alloc(size_t size)
{
   return gc_micro_alloc_ptr(size);
}

//! Micro free function.
//! @see gc_micro_alloc
//!
//! @param pointer Pointer to the block of memory to free.
GC_INLINE void gc_micro_free(void* pointer)
{
   gc_micro_free_ptr(pointer);
}

//! @}

//! Small-block memory copy. 
//!
//! This is a memcpy function which is intended for use in small size copies. 
//! The implementation behind this function should cause minimal cache pollution
//! and not do any kind of large-batch optimization. 
//!
//! @param dest Destination address.
//! @param src Source address.
//! @param size The size of memory to be copied.
//! @return The destination address.
GC_INLINE void* gc_microrcpy(void* restrict dest, const void* restrict src, size_t size)
{
   return gc_microrcpy_ptr(dest, src, size);
}

//! @cond
// Alignment pre/post-fix for VC/GCC
#if _MSC_VER
#  define GC_ALIGNPRE(x) __declspec(align(x))
#  define GC_ALIGNPOST(x) 
#else
#  define GC_ALIGNPRE(x) 
#  define GC_ALIGNPOST(x) __attribute__ ((aligned (x)))
#endif

// Struct helpers
#define GC_ALIGNED_STRUCT_PRE(alignment) typedef GC_ALIGNPRE(alignment) struct
#define GC_ALIGNED_STRUCT_POST(structname, alignment) structname GC_ALIGNPOST(alignment)
//! @endcond

#ifdef __cplusplus
}
#endif

#endif