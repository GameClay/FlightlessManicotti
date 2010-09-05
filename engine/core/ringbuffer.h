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
 
#ifndef _GC_RINGBUFFER_H_
#define _GC_RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "core/internal/ringbuffer_internal.h"

// Default ring buffer types
GC_DECLARE_RINGBUFFER_TYPE(float);
GC_DECLARE_RINGBUFFER_TYPE(int);

// Macros
#define gc_ringbuffer(T) gc_ringbuffer_##T

/// Initializes a ring-buffer using a provided buffer.
///
/// This function does not allocate any memory. A ring-buffer initialized using
/// this function should not call gc_free_ringbuffer.
///
/// @param[in] T The type of the ring-buffer 
/// @param[out] ringbuffer ring-buffer to initialize.
/// @param[in] size The size of the buffer parameter.
/// @param[in] buffer The buffer of memory used to back the ring-buffer.
/// @param[in] mutex The mutex to use for this ring-buffer.
#define gc_init_ringbuffer(T, ringbuffer, size, buffer, mtx) gc_init_ringbuffer_##T(ringbuffer, size, buffer, mtx)


/// Initializes a ring-buffer using memory allocated by the function.
///
/// Ring-buffer initialized using this function must call gc_free_ringbuffer
/// to free the associated memory.
///
/// @param[in] T The type of the ring-buffer 
/// @param[out] ringbuffer The ring-buffer to initialize.
/// @param[in] size The size of the memory to allocate.
/// @return GC_SUCCESS if the allocation was successful.
///         GC_ERROR if the allocation failed.
#define gc_alloc_ringbuffer(T, ringbuffer, size) gc_alloc_ringbuffer_##T(ringbuffer, size)

/// Frees the memory associated with a ring-buffer initialized using gc_alloc_ringbuffer.
///
/// @attention You should only call this funtion on ring-buffer initialized 
/// using gc_alloc_ringbuffer.
///
/// @param[in] T The type of the ring-buffer 
/// @param queue[out] The ring-buffer to free.
#define gc_free_ringbuffer(T, ringbuffer) gc_free_ringbuffer_##T(ringbuffer)

/// Reserve a chunk of memory in a ring-buffer.
///
/// @param[in] T The type of the ring-buffer 
/// @param[in,out] ringbuffer The ring-buffer from which to reserve the memory.
/// @param[in] item The item to enqueue.
/// @return GC_SUCCESS if successful.
///         GC_ERROR if the memory reservation failed.
#define gc_reserve_ringbuffer(T, ringbuffer, item) gc_reserve_ringbuffer_##T(ringbuffer, item)

/// Retrieve a chunk of memory from a ring-buffer.
///
/// @param[in] T The type of the ring-buffer 
/// @param[in,out] ringbuffer The ring-buffer from which to retrieve the memory.
/// @param[out] item The location to store the recovered memory.
/// @return GC_SUCCESS if the memory was recovered successfully.
///         GC_ERROR if the ring-buffer was empty.
#define gc_retrieve_ringbuffer(T, ringbuffer, item) gc_retrieve_ringbuffer_##T(ringbuffer, item)

#ifdef __cplusplus
}
#endif

#endif
