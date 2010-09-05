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

#ifndef _GC_RINGBUFFER_INTERNAL_H_
#define _GC_RINGBUFFER_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <stddef.h>
#include <amp/amp.h>

#define _GC_DECLARE_RINGBUFFER_STRUCT_(t) \
   GC_API typedef struct                  \
   {                                      \
      size_t start;                       \
      size_t end;                         \
      size_t size;                        \
      t* buffer;                          \
      amp_mutex_t mutex;                  \
   } gc_ringbuffer_##t


#define _GC_INIT_RINGBUFFER_FN_(t) void gc_init_ringbuffer_##t(gc_ringbuffer_##t* ringbuffer, size_t size, t* buffer, amp_mutex_t mtx)
#define _GC_ALLOC_RINGBUFFER_FN_(t) int gc_alloc_ringbuffer_##t(gc_ringbuffer_##t* ringbuffer, size_t size)
#define _GC_FREE_RINGBUFFER_FN_(t) void gc_free_ringbuffer_##t(gc_ringbuffer_##t* ringbuffer)
#define _GC_RESERVE_RINGBUFFER_FN_(t) int gc_reserve_ringbuffer_##t(gc_ringbuffer_##t* ringbuffer, const t* item)
#define _GC_RETRIEVE_RINGBUFFER_FN_(t) int gc_retrieve_ringbuffer_##t(gc_ringbuffer_##t* ringbuffer, t* item)

// Declare a ringbuffer type
#define GC_DECLARE_RINGBUFFER_TYPE(t)           \
   _GC_DECLARE_RINGBUFFER_STRUCT_(t);           \
   extern GC_API _GC_INIT_RINGBUFFER_FN_(t);    \
   extern GC_API _GC_ALLOC_RINGBUFFER_FN_(t);   \
   extern GC_API _GC_FREE_RINGBUFFER_FN_(t);    \
   extern GC_API _GC_RESERVE_RINGBUFFER_FN_(t); \
   extern GC_API _GC_RETRIEVE_RINGBUFFER_FN_(t)

// Implement a ringbuffer type
#define GC_IMPLEMENT_RINGBUFFER_TYPE(t)                        \
   _GC_INIT_RINGBUFFER_FN_(t)                                  \
   {                                                           \
      ringbuffer->buffer = buffer;                             \
      ringbuffer->start = 0;                                   \
      ringbuffer->end = 0;                                     \
      ringbuffer->size = size;                                 \
      ringbuffer->mutex = mtx;                                 \
   }                                                           \
   \
   _GC_ALLOC_RINGBUFFER_FN_(t)                                 \
   {                                                           \
      ringbuffer->buffer = gc_heap_alloc(size * sizeof(t), 4); \
      if(ringbuffer->buffer == NULL)                           \
         return GC_ERROR;                                      \
                                                               \
      ringbuffer->start = 0;                                   \
      ringbuffer->end = 0;                                     \
      ringbuffer->size = size;                                 \
      amp_mutex_create(&ringbuffer->mutex, AMP_DEFAULT_ALLOCATOR); \
                                                               \
      return GC_SUCCESS;                                       \
   }                                                           \
   \
   _GC_FREE_RINGBUFFER_FN_(t)                                  \
   {                                                           \
      gc_heap_free(ringbuffer->buffer);                        \
      ringbuffer->buffer = NULL;                               \
      ringbuffer->start = 0;                                   \
      ringbuffer->end = 0;                                     \
      ringbuffer->size = 0;                                    \
      amp_mutex_destroy(&ringbuffer->mutex, AMP_DEFAULT_ALLOCATOR); \
      ringbuffer->mutex = NULL;                                \
   }                                                           \
   \
   _GC_RESERVE_RINGBUFFER_FN_(t)                               \
   {                                                           \
      int ret = GC_ERROR;                                      \
      amp_mutex_lock(ringbuffer->mutex);                       \
      const size_t nend =                                      \
         (ringbuffer->end + 1) % ringbuffer->size;             \
      if(nend % ringbuffer->size != ringbuffer->start)         \
      {                                                        \
         gc_microrcpy(ringbuffer->buffer + nend, item, sizeof(t)); \
         ringbuffer->end = nend;                               \
         ret = GC_SUCCESS;                                     \
      }                                                        \
      amp_mutex_unlock(ringbuffer->mutex);                     \
      return ret;                                              \
   }                                                           \
   \
   _GC_RETRIEVE_RINGBUFFER_FN_(t)                              \
   {                                                           \
      int ret = GC_ERROR;                                      \
      amp_mutex_lock(ringbuffer->mutex);                       \
      if(ringbuffer->start != ringbuffer->end)                 \
      {                                                        \
         const size_t nstart =                                 \
            (ringbuffer->start + 1) % ringbuffer->size;        \
         gc_microrcpy(item, ringbuffer->buffer + nstart, sizeof(t)); \
         ringbuffer->start = nstart;                           \
         ret = GC_SUCCESS;                                     \
      }                                                        \
      amp_mutex_unlock(ringbuffer->mutex);                     \
      return ret;                                              \
   }

#ifdef __cplusplus
}
#endif

#endif
