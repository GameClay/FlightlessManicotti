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
 
#ifndef GC_RINGBUFFER_H_
#define GC_RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <stddef.h>
#include <stdatomic.h>

/// A lockless, thread-safe ring-buffer based queue.
GC_API GC_ALIGNED_STRUCT_PRE(16)
{
   atomic_size_t start;
   atomic_size_t end;
   atomic_size_t size;
   atomic_address buffer;
} GC_ALIGNED_STRUCT_POST(gc_ringbuffer, 16);

/// Initializes a ring-buffer using a provided buffer.
///
/// This function does not allocate any memory. A ring-buffer initialized using
/// this function should not call gc_free_ringbuffer.
///
/// @param[out] queue Ring-buffer to initialize.
/// @param[in] size The size of the buffer parameter.
/// @param[in] buffer The buffer of memory used to back the ring-buffer.
GC_API void gc_init_ringbuffer(gc_ringbuffer* ringbuffer, size_t size, void* buffer);

/// Initializes a ring-buffer using memory allocated by the function.
///
/// Ring-buffer initialized using this function must call gc_free_ringbuffer
/// to free the associated memory.
///
/// @param[out] ringbuffer The ring-buffer to initialize.
/// @param[in] size The size of the memory to allocate.
/// @return GC_SUCCESS if the allocation was successful.
///         GC_ERROR if the allocation failed.
GC_API int gc_alloc_ringbuffer(gc_ringbuffer* ringbuffer, size_t size);

/// Frees the memory associated with a ring-buffer initialized using gc_alloc_ringbuffer.
///
/// @attention You should only call this funtion on ring-buffer initialized 
/// using gc_alloc_ringbuffer.
///
/// @param queue[out] The queue to free.
GC_API void gc_free_ringbuffer(gc_ringbuffer* ringbuffer);

/// Returns a pointer to the current first element in the ring-buffer.
///
/// @note Depending on usage of your queue, this may change at any time.
///
/// @param[in] queue The queue to peek.
/// @return Pointer to the first element in the queue.
inline void* gc_peek_ringbuffer(const gc_ringbuffer* ringbuffer, size_t size)
{
   return ((char*)ringbuffer->buffer) + ringbuffer->start;
}

/// Reserve a chunk of memory in a ring-buffer.
///
/// @param[in,out] ringbuffer The ring-buffer from which to reserve the memory.
/// @param[in] size Size of memory to reserve.
/// @return A pointer to the reserved memory if successful.
///         NULL if the memory reservation failed.
GC_API void* gc_reserve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size);

/// Retrieve a chunk of memory from a ring-buffer.
///
/// @param[in,out] ringbuffer The ring-buffer from which to retrieve the memory.
/// @param[in] size The size of memory to retrieve.
/// @param[out] item The location to store the recovered memory.
/// @return An address if the memory was recovered successfully.
///         NULL if the ring-buffer was empty.
GC_API void* gc_retrieve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size);

/// Free a chunk of memory from the back of a ring-buffer.
///
/// @param[in,out] ringbuffer The ring-buffer from which to free the memory.
/// @param[in] size The size of memory to free.
/// @param[in] last_end The end of the ring buffer, as far as you know.
/// @return The size of the memory that was freed.
GC_API int gc_unreserve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size, size_t last_end);

#ifdef __cplusplus
}
#endif

#endif