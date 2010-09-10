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
 
#ifndef _KL_IDXALLOCATOR_H_
#define _KL_IDXALLOCATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct _kl_idx_allocator* kl_idx_allocator_t;

//! Allocate an index-allocator.
//!
//! @param idx_allocator Destination for newly allocated index-allocator.
extern KL_API int kl_alloc_idx_allocator(kl_idx_allocator_t* idx_allocator, uint32_t free_list_sz);

//! Free an index-allocator
//!
//! @param idx_allocator Index-allocator to be destroyed.
extern KL_API void kl_free_idx_allocator(kl_idx_allocator_t* idx_allocator);

//! Reserve an index.
//!
//! This reserves a unique index fron an index-allocator.
//!
//! @param idx_allocator The index-allocator from which to reserve an index.
//! @return A unique index value.
extern KL_API uint32_t kl_idx_allocator_reserve(kl_idx_allocator_t idx_allocator);

//! Release an index from use.
//!
//! @param idx_allocator The index-allocator from which to release an index.
//! @param  idx The index to return to the free list.
extern KL_API void kl_idx_allocator_release(kl_idx_allocator_t idx_allocator, uint32_t idx);

#ifdef __cplusplus
}
#endif

#endif