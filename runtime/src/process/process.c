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

#include <FlightlessManicotti/process/process.h>
#include <FlightlessManicotti/core/memory.h>
#include <FlightlessManicotti/core/idxallocator.h>
#include <FlightlessManicotti/scriptinterface/script.h>

struct _kl_process_manager
{
   kl_process_tick_ptr* tick;
   kl_process_advance_time_ptr* advance_time;
   void** context;
   kl_idx_allocator_t id_allocator;
   uint32_t num_processes;
   uint32_t max_id_allocated;
};

kl_process_manager_t g_process_manager = NULL;
kl_script_event_t g_advance_time_script_event;
kl_script_event_t g_tick_script_event;

int kl_alloc_process_manager(kl_process_manager_t* mgr, uint32_t num_processes)
{
   int ret = KL_ERROR;
   kl_process_manager_t pom;
   
   KL_ASSERT(mgr != NULL, "NULL manager pointer.");
   pom = kl_heap_alloc(sizeof(struct _kl_process_manager));
   
   if(pom != NULL)
   {
      /* TODO: More error checking */
      pom->tick = kl_heap_alloc(sizeof(kl_process_tick_ptr) * num_processes);
      KL_ASSERT(pom->tick != NULL, "Tick-function list allocation failed.");
      kl_zero_mem(pom->tick, sizeof(kl_process_tick_ptr) * num_processes);

      pom->advance_time = kl_heap_alloc(sizeof(kl_process_advance_time_ptr) * num_processes);
      KL_ASSERT(pom->advance_time != NULL, "AdvanceTime-function list allocation failed.");
      kl_zero_mem(pom->advance_time, sizeof(kl_process_advance_time_ptr) * num_processes);

      pom->context = kl_heap_alloc(sizeof(void**) * num_processes);
      KL_ASSERT(pom->context != NULL, "Context list allocation failed.");
      kl_zero_mem(pom->context, sizeof(void**) * num_processes);

      ret = kl_alloc_idx_allocator(&pom->id_allocator, num_processes);
      KL_ASSERT(ret == KL_SUCCESS, "Failed to allocate index allocator.");

      g_advance_time_script_event.event.id = kl_register_script_event("AdvanceTime");
      g_advance_time_script_event.event.context.as_ptr = NULL;
      g_advance_time_script_event.event.arg.as_uint32 = 0;

      g_tick_script_event.event.id = kl_register_script_event("ProcessTick");
      g_tick_script_event.event.context.as_ptr = NULL;
      g_tick_script_event.event.arg.as_uint32 = 0;

      pom->num_processes = num_processes;
      pom->max_id_allocated = 0;

      *mgr = pom;
      ret = KL_SUCCESS;
   }

   return ret;
}

void kl_free_process_manager(kl_process_manager_t* mgr)
{
   kl_process_manager_t pom;
   KL_ASSERT(mgr != NULL, "NULL manager pointer.");
   pom = *mgr;

   kl_heap_free(pom->tick);
   kl_heap_free(pom->advance_time);
   kl_heap_free(pom->context);
   kl_free_idx_allocator(&pom->id_allocator);
   kl_heap_free(pom);
   *mgr = NULL;
}

uint32_t kl_reserve_process_id(kl_process_manager_t mgr,
   kl_process_tick_ptr tick_fn, kl_process_advance_time_ptr advance_time_fn, void* context)
{
   uint32_t ret;
   kl_process_manager_t pom = (mgr == KL_DEFAULT_PROCESS_MANAGER ? g_process_manager : mgr);
   KL_ASSERT(pom != NULL, "NULL process manager.");

   ret = kl_idx_allocator_reserve(pom->id_allocator);
   pom->max_id_allocated = (ret > pom->max_id_allocated ? ret : pom->max_id_allocated);

   pom->advance_time[ret] = advance_time_fn;
   pom->tick[ret] = tick_fn;
   pom->context[ret] = context;

   return ret;
}

void kl_release_process_id(kl_process_manager_t mgr, uint32_t id)
{
   kl_process_manager_t pom = (mgr == KL_DEFAULT_PROCESS_MANAGER ? g_process_manager : mgr);
   KL_ASSERT(pom != NULL, "NULL process-object manager.");

   pom->tick[id] = NULL;
   pom->advance_time[id] = NULL;
   pom->context[id] = NULL;

   kl_idx_allocator_release(pom->id_allocator, id);
}

int kl_tick_process_list(const kl_process_manager_t mgr)
{
   kl_process_tick_ptr* tick_fn;
   void** context;
   uint32_t i;
   kl_process_manager_t pom = (mgr == KL_DEFAULT_PROCESS_MANAGER ? g_process_manager : mgr);
   KL_ASSERT(pom != NULL, "NULL process manager.");

   tick_fn = pom->tick;
   context = pom->context;
   for(i = 0; i <= pom->max_id_allocated; i++)
   {
      if(tick_fn[i] != NULL)
         tick_fn[i](context[i]);
   }

   return KL_SUCCESS;
}

int kl_advance_process_list(const kl_process_manager_t mgr, float dt)
{
   kl_process_advance_time_ptr* advance_time_fn;
   void** context;
   uint32_t i;
   kl_process_manager_t pom = (mgr == KL_DEFAULT_PROCESS_MANAGER ? g_process_manager : mgr);

   KL_ASSERT(pom != NULL, "NULL process manager.");
   advance_time_fn = pom->advance_time;
   context = pom->context;

   for(i = 0; i <= pom->max_id_allocated; i++)
   {
      if(advance_time_fn[i] != NULL)
         advance_time_fn[i](dt, context[i]);
   }

   return KL_SUCCESS;
}
