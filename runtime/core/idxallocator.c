/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2011 GameClay LLC
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
   int ret = KL_ERROR;
   const uint32_t alloc_sz = sizeof(struct _kl_idx_allocator) + sizeof(uint32_t) * free_list_sz;
   struct _kl_idx_allocator* idxalloc = NULL;
   int i;
   
   KL_ASSERT(idx_allocator != NULL, "NULL index-allocator pointer");
   
   idxalloc = (struct _kl_idx_allocator*)kl_heap_alloc(alloc_sz);
   
   if(idxalloc != NULL)
   {
      idxalloc->free_list_sz = free_list_sz;
      idxalloc->free_tail_idx = free_list_sz;
      idxalloc->free_list = (uint32_t*)(((char*)idxalloc) + sizeof(struct _kl_idx_allocator));
      
      // Prep free-list
      for(i = 0; i < free_list_sz; i++)
      {
         idxalloc->free_list[i] = i;
      }
      
      *idx_allocator = idxalloc;
      ret = KL_SUCCESS;
   }
   
   return ret;
}

void kl_free_idx_allocator(kl_idx_allocator_t* idx_allocator)
{
   KL_ASSERT(idx_allocator != NULL, "NULL index-allocator pointer");
   KL_ASSERT(*idx_allocator != NULL, "NULL index-allocator");
   
   kl_heap_free(*idx_allocator);
}

uint32_t kl_idx_allocator_reserve(kl_idx_allocator_t idx_allocator)
{
   uint32_t ret = UINT32_MAX;
   struct _kl_idx_allocator* idxalloc = idx_allocator;
   KL_ASSERT(idxalloc != NULL, "NULL index-allocator");
   
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
   uint32_t i = 0;
   struct _kl_idx_allocator* idxalloc = idx_allocator;
   KL_ASSERT(idxalloc != NULL, "NULL index-allocator");
   KL_ASSERT(idx < idxalloc->free_list_sz, "Index out of range for index-allocator.");
   
#ifdef KL_ENABLE_ASSERTS
   for(i = 0; i < idxalloc->free_tail_idx; i++)
      KL_ASSERT(idxalloc->free_list[i] != idx, "Index was freed twice.");
#endif
   
   if(idxalloc->free_tail_idx < idxalloc->free_list_sz)
   {
      idxalloc->free_list[idxalloc->free_tail_idx] = idx;
      idxalloc->free_tail_idx++;
   }
}
