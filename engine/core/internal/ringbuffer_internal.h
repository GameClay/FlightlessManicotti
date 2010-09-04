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

// Note that there is no include guard, this is intentional

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <stddef.h>

#define _GC_DECLARE_RINGBUFFER_STRUCT_(t) \
   GC_API typedef struct                  \
   {                                      \
      size_t start;                       \
      size_t end;                         \
      size_t size;                        \
      t* buffer;                          \
   } gc_ringbuffer_##t


#define _GC_INIT_RINGBUFFER_FN_(t) void gc_init_ringbuffer_##t(gc_ringbuffer_##t* ringbuffer, size_t size, t* buffer)
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
   }                                                           \
   \
   _GC_RESERVE_RINGBUFFER_FN_(t)                               \
   {                                                           \
      const size_t nend =                                      \
         (ringbuffer->end + 1) % ringbuffer->size;             \
      if(nend % ringbuffer->size != ringbuffer->start)         \
      {                                                        \
         ringbuffer->buffer[nend] = *item;                     \
         ringbuffer->end = nend;                               \
         return GC_SUCCESS;                                    \
      }                                                        \
      return GC_ERROR;                                         \
   }                                                           \
   \
   _GC_RETRIEVE_RINGBUFFER_FN_(t)                              \
   {                                                           \
      if(ringbuffer->start != ringbuffer->end)                 \
      {                                                        \
         const size_t nstart =                                 \
            (ringbuffer->start + 1) % ringbuffer->size;        \
         *item = ringbuffer->buffer[nstart];                   \
         ringbuffer->start = nstart;                           \
         return GC_SUCCESS;                                    \
      }                                                        \
      return GC_ERROR;                                         \
   }

#ifdef __cplusplus
}
#endif
