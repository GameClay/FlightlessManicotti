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
 
#include "process/process.h"
#include "core/memory.h"
#include "core/idxallocator.h"

struct _kl_process_object_manager
{
   kl_process_object_tick_ptr* tick;
   kl_process_object_advance_time_ptr* advance_time;
   kl_idx_allocator_t id_allocator;
   uint32_t num_objects;
};

int kl_alloc_process_object_manager(kl_process_object_manager_t* mgr, uint32_t num_objects)
{
   int ret = KL_ERROR;
   kl_process_object_manager_t pom = kl_heap_alloc(num_objects * sizeof(struct _kl_process_object_manager));
   
   if(pom != NULL)
   {
      pom->tick = kl_heap_alloc(sizeof(kl_process_object_tick_ptr) * num_objects);
      pom->advance_time = kl_heap_alloc(sizeof(kl_process_object_advance_time_ptr) * num_objects);
      ret = kl_alloc_idx_allocator(&pom->id_allocator, num_objects);
      KL_ASSERT(ret == KL_SUCCESS, "Failed to allocate index allocator.");
      pom->num_objects = num_objects;

      *mgr = pom;
      ret = KL_SUCCESS;
   }
   
   return ret;
}

void kl_free_process_object_manager(kl_process_object_manager_t* mgr)
{
   kl_process_object_manager_t pom = *mgr;
   kl_heap_free(pom->tick);
   kl_heap_free(pom->advance_time);
   kl_free_idx_allocator(&pom->id_allocator);
   kl_heap_free(pom);
   *mgr = NULL;
}

uint32_t kl_reserve_process_object_id(kl_process_object_manager_t mgr)
{
   return kl_idx_allocator_reserve(mgr->id_allocator);
}

void kl_release_process_object_id(kl_process_object_manager_t mgr, uint32_t id)
{
   kl_idx_allocator_release(mgr->id_allocator, id);
}

int kl_tick_process_object_list(const kl_process_object_manager_t mgr)
{
   kl_process_object_tick_ptr* tick_fn = mgr->tick;
   int i;
   
   for(i = 0; i < mgr->num_objects; i++)
   {
      tick_fn[i]();
   }
   
   return KL_SUCCESS;
}

int kl_advance_process_object_list(const kl_process_object_manager_t mgr, float dt)
{
   kl_process_object_advance_time_ptr* advance_time_fn = mgr->advance_time;
   int i;
   
   for(i = 0; i < mgr->num_objects; i++)
   {
      advance_time_fn[i](dt);
   }
   
   return KL_SUCCESS;   
}