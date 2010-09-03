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
 
#include "core/queue.h"

void gc_init_queue(gc_queue* queue, size_t element_size, size_t num_elements, void* buffer)
{
   queue->buffer = buffer;
   queue->start = 0;
   queue->end = 0;
   queue->element_size = element_size;
   queue->size = num_elements;
}

int gc_alloc_queue(gc_queue* queue, size_t element_size, size_t num_elements)
{
   queue->buffer = gc_heap_alloc(element_size * num_elements, 16);
   
   if(queue->buffer == NULL)
      return GC_ERROR;
   
   queue->start = 0;
   queue->end = 0;
   queue->element_size = element_size;
   queue->size = num_elements;
   return GC_SUCCESS;
}

void gc_free_queue(gc_queue* queue)
{
   gc_heap_free(queue->buffer);
   queue->buffer = NULL;
   queue->start = 0;
   queue->end = 0;
   queue->size = 0;
   queue->element_size = 0;
}

int gc_enqueue(gc_queue* queue, const void* item)
{
   size_t qend = queue->end;
   size_t end = (qend + 1) % queue->size;
   void* dest = ((char*)queue->buffer) + end * queue->element_size;
   if((qend+ 1) % queue->size != queue->start) 
   {
      if(atomic_compare_exchange_weak(&queue->end, &qend, end))
      {
         gc_microrcpy(dest, item, queue->element_size);
         return GC_SUCCESS;
      }
      return GC_RETRY;
   }
   return GC_ERROR;
}

int gc_dequeue(gc_queue* queue, void* item)
{
   size_t qstart = queue->start;
   size_t start = (qstart + 1) % queue->size;
   void* entry = ((char*)queue->buffer) + start * queue->element_size;
   if(qstart % queue->size != queue->end) 
   {
      if(atomic_compare_exchange_weak(&queue->start, &qstart, start))
      {
         gc_microrcpy(item, entry, queue->element_size);
         return GC_SUCCESS;
      }
      return GC_RETRY;
   }
   return GC_ERROR;
}