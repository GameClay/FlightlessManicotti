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

KL_IMPLEMENT_RINGBUFFER_TYPE(float);
KL_IMPLEMENT_RINGBUFFER_TYPE(int);

// Custom implementation of __m128i type
_KL_INIT_RINGBUFFER_FN_(__m128i)
{
   ringbuffer->buffer = buffer;
   ringbuffer->start = 0;
   ringbuffer->end = 0;
   ringbuffer->size = size / sizeof(__m128i);
   ringbuffer->mutex = mtx;
}

_KL_ALLOC_RINGBUFFER_FN_(__m128i)
{
   ringbuffer->buffer = (__m128i*)kl_heap_alloc(size * sizeof(__m128i));
   if(ringbuffer->buffer == NULL)
      return KL_ERROR;

   ringbuffer->start = 0;
   ringbuffer->end = 0;
   ringbuffer->size = size;
   amp_mutex_create(&ringbuffer->mutex, AMP_DEFAULT_ALLOCATOR); 

   return KL_SUCCESS;
}

_KL_FREE_RINGBUFFER_FN_(__m128i)
{
   kl_heap_free(ringbuffer->buffer);
   ringbuffer->buffer = NULL;
   ringbuffer->start = 0;
   ringbuffer->end = 0;
   ringbuffer->size = 0;
   amp_mutex_destroy(&ringbuffer->mutex, AMP_DEFAULT_ALLOCATOR);
   ringbuffer->mutex = NULL;
}

_KL_RESERVE_RINGBUFFER_FN_(__m128i)
{
   int ret = KL_ERROR;
   uint32_t nend;
   
   amp_mutex_lock(ringbuffer->mutex);
   nend = (ringbuffer->end + 1) % ringbuffer->size;
   if(nend != ringbuffer->start)
   {
      __m128i xmm0 = _mm_load_si128(item);
      _mm_store_si128(ringbuffer->buffer + ringbuffer->end, xmm0);
      
      ringbuffer->end = nend;
      ret = KL_SUCCESS;
   }
   amp_mutex_unlock(ringbuffer->mutex);
   
   return ret;
}

_KL_RETRIEVE_RINGBUFFER_FN_(__m128i)
{
   int ret = KL_ERROR;
   amp_mutex_lock(ringbuffer->mutex);
   if(ringbuffer->start != ringbuffer->end)
   { 
      __m128i xmm0 = _mm_load_si128(ringbuffer->buffer + ringbuffer->start);
      _mm_store_si128(item, xmm0);
      
      ringbuffer->start = (ringbuffer->start + 1) % ringbuffer->size;
      ret = KL_SUCCESS;
   }
   amp_mutex_unlock(ringbuffer->mutex);
   
   return ret;
}