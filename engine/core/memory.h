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
 
#ifndef GC_MEMORY_H_
#define GC_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "ccompat.h"

typedef void* (*gc_aligned_malloc_fn_ptr)(size_t size, size_t align_size);
typedef void (*gc_aligned_free_fn_ptr)(void* pointer);
typedef void* (*gc_memrcpy_fn_ptr)(void* restrict dest, const void* restrict src, size_t size);

/// Aligned malloc function.
///
/// @see gc_aligned_malloc_fn_ptr
///
/// @note The amount of allocated memory may exceed the size specified.
///
/// @param[in] size The size of the allocation.
/// @param[in] align_size The boundary on which the allocated memory should be aligned.
/// @return Pointer the block of allocated memory, aligned on the specified boundary,
///         or NULL if the allocation failed.
extern gc_aligned_malloc_fn_ptr gc_heap_alloc;

/// Aligned free function.
///
/// @see gc_aligned_free_fn_ptr
///
/// @param[in] pointer Pointer to the block of memory to free.
extern gc_aligned_free_fn_ptr gc_heap_free;

/// Small-block memory copy. 
///
/// This is a memcpy function which is intended for use in small size copies. 
/// The implementation behind this function should cause minimal cache pollution
/// and not do any kind of large-batch optimization. 
///
/// @see gc_memrcpy_fn_ptr
///
/// @param[out] dest Destination address.
/// @param[in] src Source address.
/// @param[in] size The size of memory to be copied.
/// @return The destination address.
extern gc_memrcpy_fn_ptr gc_microrcpy;

// Alignment pre/post-fix for VC/GCC
#if 0 // Visual Studio
#  define GC_ALIGNPRE(x) __declspec(align(x))
#  define GC_ALIGNPOST(x) 
#else
#  define GC_ALIGNPRE(x) 
#  define GC_ALIGNPOST(x) __attribute__ ((aligned (x)))
#endif

// Struct helpers
#define GC_ALIGNED_STRUCT_PRE(alignment) typedef GC_ALIGNPRE(alignment) struct
#define GC_ALIGNED_STRUCT_POST(structname, alignment) structname GC_ALIGNPOST(alignment)

#ifdef __cplusplus
}
#endif

#endif