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

// Function pointer types
typedef void* (*_gcaligned_malloc_fn_ptr)(size_t size, size_t align_size);
typedef void (*_gcaligned_free_fn_ptr)(void* pointer);
typedef void* (*_gcmemrcpy_fn_ptr)(void* __restrict dest, const void* __restrict src, size_t size);

extern _gcaligned_malloc_fn_ptr gcheap_alloc;
extern _gcaligned_free_fn_ptr gcheap_free;
extern _gcmemrcpy_fn_ptr gcmicrorcpy;

// Alignment pre/post-fix for VC/GCC
#if 0 // Visual Studio
#  define GCALIGNPRE(x) __declspec(align(x))
#  define GCALIGNPOST(x) 
#else
#  define GCALIGNPRE(x) 
#  define GCALIGNPOST(x) __attribute__ ((aligned (x)))
#endif

// Struct helpers
#define GCALIGNED_STRUCT_PRE(alignment) typedef GCALIGNPRE(alignment) struct
#define GCALIGNED_STRUCT_POST(structname, alignment) structname GCALIGNPOST(alignment)

#ifdef __cplusplus
}
#endif

#endif