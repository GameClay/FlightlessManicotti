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

#include "game/scene2d/scene_container_2d.h"
#include "core/idxallocator.h"


// Forward declare process manager callbacks
void _kl_scene_container_2d_process_tick(void* context);
void _kl_scene_container_2d_advance_time(float dt, void* context);

// Opaque scene container
struct _kl_scene_container_2d
{
   uint32_t pid;
   kl_process_manager_t process_manager;
};

int kl_alloc_scene_container_2d(kl_scene_container_2d_t* container, kl_process_manager_t process_manager)
{
   int ret = KL_ERROR;
   kl_scene_container_2d_t sctr;
   
   KL_ASSERT(container != NULL, "NULL container pointer.");
   sctr = kl_heap_alloc(sizeof(struct _kl_scene_container_2d));
   
   if(sctr != NULL)
   {
      sctr->process_manager = process_manager;
      sctr->pid = kl_reserve_process_id(process_manager, NULL, NULL, sctr);
      *container = sctr;
      
      ret = KL_SUCCESS;
   }
   
   return ret;
}

void kl_free_scene_container_2d(kl_scene_container_2d_t* container)
{
   kl_scene_container_2d_t sctr = *container;
   kl_release_process_id(sctr->process_manager, sctr->pid);
   kl_heap_free(sctr);
   *container = NULL;
}

// Internal callbacks for process manager
void _kl_scene_container_2d_process_tick(void* context)
{
   kl_scene_container_2d_t sctr = (kl_scene_container_2d_t)context;
   KL_UNUSED(sctr);
}

void _kl_scene_container_2d_advance_time(float dt, void* context)
{
   kl_scene_container_2d_t sctr = (kl_scene_container_2d_t)context;
   KL_UNUSED(sctr);
}