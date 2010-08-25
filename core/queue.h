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
#include "inline.h"

GCALIGNED_STRUCT_PRE(16)
{
   atomic_size_t start;
   atomic_size_t end;
   atomic_size_t size;
   atomic_size_t element_size;
   atomic_address buffer;
   // pad?
} GCALIGNED_STRUCT_POST(gcqueue, 16);

void gcinit_queue(gcqueue* queue, size_t element_size, size_t num_elements, void* buffer);
void gcalloc_queue(gcqueue* queue, size_t element_size, size_t num_elements);
void gcfree_queue(gcqueue* queue);

inline void* gcpeek_queue(const gcqueue* queue)
{
   return ((char*)queue->buffer) + queue->start * queue->element_size;
}

int gcenqueue(gcqueue* queue, void* item);
int gcdequeue(gcqueue* queue, void* item);

#ifdef __cplusplus
}
#endif

#endif