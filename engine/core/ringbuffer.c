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
 
#include "core/ringbuffer.h"

void gc_init_ringbuffer(gc_ringbuffer* ringbuffer, size_t size, void* buffer)
{
   ringbuffer->buffer = buffer;
   ringbuffer->start = 0;
   ringbuffer->end = 0;
   ringbuffer->size = size;
}

int gc_alloc_ringbuffer(gc_ringbuffer* ringbuffer, size_t size)
{
   ringbuffer->buffer = gc_heap_alloc(size, 16);
   if(ringbuffer->buffer == NULL)
      return GC_ERROR;
      
   ringbuffer->start = 0;
   ringbuffer->end = 0;
   ringbuffer->size = size;
   
   return GC_SUCCESS;
}

void gc_free_ringbuffer(gc_ringbuffer* ringbuffer)
{
   gc_heap_free(ringbuffer->buffer);
   ringbuffer->buffer = NULL;
   ringbuffer->start = 0;
   ringbuffer->end = 0;
   ringbuffer->size = 0;
}

void* gc_reserve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size)
{
   size_t bend = ringbuffer->end;
   size_t end = (bend + size) % ringbuffer->size;
   void* dest = ((char*)ringbuffer->buffer) + end + size;
   if((bend + 1) % ringbuffer->size != ringbuffer->start) 
   {
      while(!atomic_compare_exchange_weak(&ringbuffer->end, &bend, end))
         ;
      return dest;
   }
   return NULL;
}

int gc_retrieve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size, void* item)
{
   size_t bstart = ringbuffer->start;
   size_t start = (bstart + size) % ringbuffer->size;
   void* entry = ((char*)ringbuffer->buffer) + start + size;
   if(bstart % ringbuffer->size != ringbuffer->end) 
   {
      while(!atomic_compare_exchange_weak(&ringbuffer->start, &bstart, start))
         ;
      gc_microrcpy(item, entry, size);
      return GC_SUCCESS;
   }
   return GC_ERROR;
}