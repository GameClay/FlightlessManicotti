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
   uint32_t free_tail_idx;
   uint32_t free_list_sz;
   uint32_t* free_list;
};

int kl_alloc_idx_allocator(kl_idx_allocator_t* idx_allocator, uint32_t free_list_sz)
{
   assert(idx_allocator != NULL);
   
   int ret = KL_ERROR;
   const uint32_t alloc_sz = sizeof(struct _kl_idx_allocator) + sizeof(uint32_t) * free_list_sz;
   struct _kl_idx_allocator* idxalloc = (struct _kl_idx_allocator*)kl_heap_alloc(alloc_sz);
   
   if(idxalloc != NULL)
   {
      idxalloc->free_list_sz = free_list_sz;
      idxalloc->free_tail_idx = free_list_sz;
      idxalloc->free_list = (uint32_t*)(((char*)idxalloc) + sizeof(struct _kl_idx_allocator));
      
      // Prep free-list
      while(free_list_sz > 0)
      {
         free_list_sz--;
         idxalloc->free_list[free_list_sz] = free_list_sz;
      }
      
      *idx_allocator = idxalloc;
      ret = KL_SUCCESS;
   }
   
   return ret;
}

void kl_free_idx_allocator(kl_idx_allocator_t* idx_allocator)
{
   assert(idx_allocator != NULL);
   assert(*idx_allocator != NULL);
   
   kl_heap_free(*idx_allocator);
}

uint32_t kl_idx_allocator_reserve(kl_idx_allocator_t idx_allocator)
{
   uint32_t ret = UINT32_MAX;
   struct _kl_idx_allocator* idxalloc = idx_allocator;
   assert(idxalloc != NULL);
   
   if(idxalloc->free_tail_idx > 0)
   {
      // TODO: Use compxchng.
      idxalloc->free_tail_idx--;
      ret = idxalloc->free_list[idxalloc->free_tail_idx];
   }
   
   return ret;
}

void kl_idx_allocator_release(kl_idx_allocator_t idx_allocator, uint32_t idx)
{
   struct _kl_idx_allocator* idxalloc = idx_allocator;
   assert(idxalloc != NULL);
   assert(idx < idxalloc->free_list_sz);
   
   if(idxalloc->free_tail_idx < idxalloc->free_list_sz)
   {
      idxalloc->free_list[idxalloc->free_tail_idx] = idx;
      idxalloc->free_tail_idx++;
   }
}
