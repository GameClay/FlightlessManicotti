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

GC_ALIGNED_STRUCT_PRE(16)
{
   atomic_size_t start;
   atomic_size_t end;
   atomic_size_t size;
   atomic_size_t element_size;
   atomic_address buffer;
   // pad?
} GC_ALIGNED_STRUCT_POST(gc_queue, 16);

void gc_init_queue(gc_queue* queue, size_t element_size, size_t num_elements, void* buffer);
void gc_alloc_queue(gc_queue* queue, size_t element_size, size_t num_elements);
void gc_free_queue(gc_queue* queue);

inline void* gc_peek_queue(const gc_queue* queue)
{
   return ((char*)queue->buffer) + queue->start * queue->element_size;
}

int gc_enqueue(gc_queue* queue, void* item);
int gc_dequeue(gc_queue* queue, void* item);

#ifdef __cplusplus
}
#endif

#endif