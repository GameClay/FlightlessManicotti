/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2012 GameClay LLC
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

#ifndef _KL_RINGBUFFER_INTERNAL_H_
#define _KL_RINGBUFFER_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <stdint.h>
#include <amp/amp.h>

#define _KL_DECLARE_RINGBUFFER_STRUCT_(t) \
   typedef struct                         \
   {                                      \
      uint32_t start;                     \
      uint32_t end;                       \
      uint32_t size;                      \
      uint32_t _padding;                  \
      t* buffer;                          \
      amp_mutex_t mutex;                  \
   } kl_ringbuffer_##t##_t


#define _KL_INIT_RINGBUFFER_FN_(t) void kl_init_ringbuffer_##t(kl_ringbuffer_##t##_t* ringbuffer, uint32_t size, t* buffer, amp_mutex_t mtx)
#define _KL_ALLOC_RINGBUFFER_FN_(t) int kl_alloc_ringbuffer_##t(kl_ringbuffer_##t##_t* ringbuffer, uint32_t size)
#define _KL_FREE_RINGBUFFER_FN_(t) void kl_free_ringbuffer_##t(kl_ringbuffer_##t##_t* ringbuffer)
#define _KL_RESERVE_RINGBUFFER_FN_(t) int kl_reserve_ringbuffer_##t(kl_ringbuffer_##t##_t* ringbuffer, const t* item)
#define _KL_RETRIEVE_RINGBUFFER_FN_(t) int kl_retrieve_ringbuffer_##t(kl_ringbuffer_##t##_t* ringbuffer, t* item)

/* Declare a ringbuffer type */
#define KL_DECLARE_RINGBUFFER_TYPE(t)           \
   _KL_DECLARE_RINGBUFFER_STRUCT_(t);           \
   extern KL_API _KL_INIT_RINGBUFFER_FN_(t);    \
   extern KL_API _KL_ALLOC_RINGBUFFER_FN_(t);   \
   extern KL_API _KL_FREE_RINGBUFFER_FN_(t);    \
   extern KL_API _KL_RESERVE_RINGBUFFER_FN_(t); \
   extern KL_API _KL_RETRIEVE_RINGBUFFER_FN_(t)

/* Implement a ringbuffer type */
#define KL_IMPLEMENT_RINGBUFFER_TYPE(t)                        \
   _KL_INIT_RINGBUFFER_FN_(t)                                  \
   {                                                           \
      ringbuffer->buffer = buffer;                             \
      ringbuffer->start = 0;                                   \
      ringbuffer->end = 0;                                     \
      ringbuffer->size = size / sizeof(t);                     \
      ringbuffer->mutex = mtx;                                 \
   }                                                           \
   \
   _KL_ALLOC_RINGBUFFER_FN_(t)                                 \
   {                                                           \
      ringbuffer->buffer = (t*)kl_heap_alloc(size * sizeof(t)); \
      if(ringbuffer->buffer == NULL)                           \
         return KL_ERROR;                                      \
                                                               \
      ringbuffer->start = 0;                                   \
      ringbuffer->end = 0;                                     \
      ringbuffer->size = size;                                 \
      amp_mutex_create(&ringbuffer->mutex, AMP_DEFAULT_ALLOCATOR); \
                                                               \
      return KL_SUCCESS;                                       \
   }                                                           \
   \
   _KL_FREE_RINGBUFFER_FN_(t)                                  \
   {                                                           \
      kl_heap_free(ringbuffer->buffer);                        \
      ringbuffer->buffer = NULL;                               \
      ringbuffer->start = 0;                                   \
      ringbuffer->end = 0;                                     \
      ringbuffer->size = 0;                                    \
      amp_mutex_destroy(&ringbuffer->mutex, AMP_DEFAULT_ALLOCATOR); \
      ringbuffer->mutex = NULL;                                \
   }                                                           \
   \
   _KL_RESERVE_RINGBUFFER_FN_(t)                               \
   {                                                           \
      int ret = KL_ERROR;                                      \
      uint32_t nend;                                           \
      amp_mutex_lock(ringbuffer->mutex);                       \
      nend = (ringbuffer->end + 1) % ringbuffer->size;         \
      if(nend != ringbuffer->start)                            \
      {                                                        \
         ringbuffer->buffer[ringbuffer->end] = *item;          \
         ringbuffer->end = nend;                               \
         ret = KL_SUCCESS;                                     \
      }                                                        \
      amp_mutex_unlock(ringbuffer->mutex);                     \
      return ret;                                              \
   }                                                           \
   \
   _KL_RETRIEVE_RINGBUFFER_FN_(t)                              \
   {                                                           \
      int ret = KL_ERROR;                                      \
      amp_mutex_lock(ringbuffer->mutex);                       \
      if(ringbuffer->start != ringbuffer->end)                 \
      {                                                        \
         *item = ringbuffer->buffer[ringbuffer->start];        \
         ringbuffer->start = (ringbuffer->start + 1) % ringbuffer->size; \
         ret = KL_SUCCESS;                                     \
      }                                                        \
      amp_mutex_unlock(ringbuffer->mutex);                     \
      return ret;                                              \
   }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
