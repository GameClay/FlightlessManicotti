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
 
#ifndef GC_QUEUE_H_
#define GC_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdatomic.h>
#include "core/memory.h"

/// A lockless, thread-safe ring-buffer based queue.
GC_ALIGNED_STRUCT_PRE(16)
{
   atomic_size_t start;
   atomic_size_t end;
   atomic_size_t size;
   atomic_size_t element_size;
   atomic_address buffer;
   // pad?
} GC_ALIGNED_STRUCT_POST(gc_queue, 16);

/// Initializes a queue using a provided buffer.
///
/// This function does not allocate any memory. A queue initialized using
/// this function should not call gc_free_queue.
///
/// @param queue Queue to initialize.
/// @param element_size The size of each element in the queue.
/// @param max_num_elements The maximum number of elements in the queue.
/// @param buffer The buffer of memory which should be used to store the queue.
void gc_init_queue(gc_queue* queue, size_t element_size, size_t max_num_elements, void* buffer);

/// Initializes a queue using memory allocated by the function.
///
/// Queues initialized using this function must call gc_free_queue to free the
/// associated memory.
///
/// @param queue The queue to initialize.
/// @param element_size The size of each element in the queue.
/// @param max_num_elements The maximum number of elements in the queue.
/// @return GC_SUCCESS if the allocation was successful.
///         GC_ERROR if the allocation failed.
int gc_alloc_queue(gc_queue* queue, size_t element_size, size_t max_num_elements);

/// Frees the memory associated with a queue initialized using gc_alloc_queue.
///
/// @attention You should only call this funtion on queues initialized using gc_alloc_queue.
///
/// @param queue The queue to free.
void gc_free_queue(gc_queue* queue);

/// Returns a pointer to the current first element in the queue.
///
/// @note Depending on usage of your queue, this may change at any time.
///
/// @param queue The queue to peek.
/// @return Pointer to the first element in the queue.
inline void* gc_peek_queue(const gc_queue* queue)
{
   return ((char*)queue->buffer) + queue->start * queue->element_size;
}

/// Enqueue an item.
///
/// This function can fail with a GC_RETRY code. If you want to ensure
/// that the function succeeds you should use a while loop.
/// @code
/// while(gc_enqueue(&some_queue, &some_item) == GC_RETRY)
///    ;
/// @endcode
///
/// @param queue The queue on which to add an item.
/// @param item The item to enqueue.
/// @return GC_SUCCESS if the item was enqueued successfully.
///         GC_RETRY if the caller should retry.
///         GC_ERROR if the queue is full.
int gc_enqueue(gc_queue* queue, const void* item);

/// Dequeue an item.
///
/// This function can fail with a GC_RETRY code. If you want to ensure
/// that the function succeeds you should use a while loop.
/// @code
/// while(gc_dequeue(&some_queue, &some_item) == GC_RETRY)
///    ;
/// @endcode
///
/// @param queue The queue on which to remove an item.
/// @param item The location to store the dequeued item.
/// @return GC_SUCCESS if the item was dequeued successfully.
///         GC_RETRY if the caller should retry.
///         GC_ERROR if the queue is empty.
int gc_dequeue(gc_queue* queue, void* item);

#ifdef __cplusplus
}
#endif

#endif