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
 
#ifndef GC_STDATOMIC_H_
#define GC_STDATOMIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "core/memory.h"

// If GCC
static bool atomic_compare_exchange_weak(volatile size_t* restrict object, size_t* restrict expected, size_t desired)
{
   return __sync_bool_compare_and_swap(object, *expected, desired);
}
// Endif

// Atomic types:
// NOTE: these are only aligned versions of their normal types with these typedefs
// they do *not* have all of the 
typedef GC_ALIGNPRE(16) size_t GC_ALIGNPOST(16) atomic_size_t;

typedef GC_ALIGNPRE(16) void* GC_ALIGNPOST(16) atomic_address;

#ifdef __cplusplus
}
#endif

#endif