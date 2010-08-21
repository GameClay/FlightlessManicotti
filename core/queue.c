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
 
#include "core/queue.h"
#include "core/memory.h"
#include "core/atomic.h"

void gcinit_queue(gcqueue* queue, size_t element_size, size_t num_elements, void* buffer)
{
   queue->buffer = buffer;
   queue->start = 0;
   queue->end = 0;
   queue->element_size = element_size;
   queue->size = num_elements;
}

void gcalloc_queue(gcqueue* queue, size_t element_size, size_t num_elements)
{
   queue->buffer = gcheap_alloc(element_size * num_elements, 16);
   queue->start = 0;
   queue->end = 0;
   queue->element_size = element_size;
   queue->size = num_elements;
}

void gcfree_queue(gcqueue* queue)
{
   gcheap_free(queue->buffer);
   queue->buffer = NULL;
   queue->start = 0;
   queue->end = 0;
   queue->size = 0;
   queue->element_size = 0;
}

int gcenqueue(gcqueue* queue, void* item)
{
   size_t qend = queue->end;
   size_t end = (qend + 1) % queue->size;
   void* dest = ((char*)queue->buffer) + end * queue->element_size;
   if((qend+ 1) % queue->size != queue->start) 
   {
      if(atomic_compare_exchange_weak(&queue->end, &qend, end))
      {
         gcmicrorcpy(dest, item, queue->element_size);
         return 1;
      }
      return -1;
   }
   return 0;
}

int gcdequeue(gcqueue* queue, void* item)
{
   size_t qstart = queue->start;
   size_t start = (qstart + 1) % queue->size;
   void* entry = ((char*)queue->buffer) + start * queue->element_size;
   if(qstart % queue->size != queue->end) 
   {
      if(atomic_compare_exchange_weak(&queue->start, &qstart, start))
      {
         gcmicrorcpy(item, entry, queue->element_size);
         return 1;
      }
      return -1;
   }
   return 0;
}