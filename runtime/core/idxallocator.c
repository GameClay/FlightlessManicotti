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

#include <assert.h>
#include "fm.h"
#include "core/idxallocator.h"
#include "core/ringbuffer.h"

struct _kl_idx_allocator
{
   size_t max_idx;
   size_t free_tail_idx;
   size_t* free_list;
};

int kl_alloc_idx_allocator(kl_idx_allocator_t* idx_allocator, size_t max_idx)
{
   assert(idx_allocator != NULL);
   
   int ret = KL_ERROR;
   const size_t alloc_sz = sizeof(struct _kl_idx_allocator) + sizeof(size_t) * max_idx;
   struct _kl_idx_allocator* idxalloc = (struct _kl_idx_allocator*)kl_heap_alloc(alloc_sz);
   
   if(*idx_allocator != NULL)
   {
      idxalloc->max_idx = max_idx;
      idxalloc->free_tail_idx = 0;
      idxalloc->free_list = (struct _kl_idx_allocator*)((char*)idxalloc + sizeof(struct _kl_idx_allocator));
      *idx_allocator = idxalloc;
      ret = KL_SUCCESS;
   }
   
   return ret;
}


void kl_free_idx_allocator(kl_idx_allocator_t* idx_allocator)
{
   assert(idx_allocator != NULL);
   assert(*idx_allocator != NULL);
   
   kl_heap_free((char*)*idx_allocator - sizeof(struct _kl_idx_allocator));
}
