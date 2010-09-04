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
   void* dest = ((char*)ringbuffer->buffer) + end;
   if((bend + 1) % ringbuffer->size != ringbuffer->start) 
   {
      while(!atomic_compare_exchange_weak(&ringbuffer->end, &bend, end))
         ;
      return dest;
   }
   return NULL;
}

void* gc_retrieve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size)
{
   size_t bstart, start;
   void* entry;
   
gc_rtr_start:
   bstart = ringbuffer->start;
   start = (bstart + size) % ringbuffer->size;
   entry = ((char*)ringbuffer->buffer) + start;
   if(bstart % ringbuffer->size != ringbuffer->end) 
   {
      if(!atomic_compare_exchange_weak(&ringbuffer->start, &bstart, start))
         goto gc_rtr_start;
      return entry;
   }
   return NULL;
}

int gc_unreserve_ringbuffer(gc_ringbuffer* ringbuffer, size_t size, size_t last_end)
{
   size_t end = (last_end - size) % ringbuffer->size;
   void* dest = ((char*)ringbuffer->buffer) + end;
   if((last_end + 1) % ringbuffer->size != ringbuffer->start) 
   {
      if(!atomic_compare_exchange_weak(&ringbuffer->end, &last_end, end))
         return 0;
      return size;
   }
   return 0;
}