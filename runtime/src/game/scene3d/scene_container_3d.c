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

#include <math.h>
#include <float.h>
#include <FlightlessManicotti/game/scene3d/scene_container_3d.h>

/* Forward declare process manager callbacks */
void _kl_scene_container_3d_process_tick(void* context);
void _kl_scene_container_3d_advance_time(float dt, void* context);

int kl_alloc_scene_container_3d(kl_scene_container_3d_t* container, 
   kl_process_manager_t process_manager, uint32_t max_entries)
{
   int ret = KL_ERROR;
   kl_scene_container_3d_t sctr;

   KL_ASSERT(container != NULL, "NULL container pointer.");
   sctr = kl_heap_alloc(sizeof(_kl_scene_container_3d));

   if(sctr != NULL)
   {
      sctr->max_entries = max_entries;

      ret = kl_alloc_idx_allocator(&sctr->id_allocator, max_entries);
      KL_ASSERT(ret == KL_SUCCESS, "Failed to allocate index allocator.");

      sctr->transform = kl_heap_alloc(sizeof(float) * 16 * max_entries);
      KL_ASSERT(sctr->transform != NULL, "Failed to allocate transform stream.");

      sctr->typemask = kl_heap_alloc(sizeof(uint32_t) * max_entries);
      KL_ASSERT(sctr->typemask != NULL, "Failed to allocate typemask stream.");

      sctr->radius = kl_heap_alloc(sizeof(float) * max_entries);
      KL_ASSERT(sctr->typemask != NULL, "Failed to allocate radius stream.");

      sctr->reference = kl_heap_alloc(sizeof(int) * max_entries);
      KL_ASSERT(sctr->reference != NULL, "Failed to allocate Lua reference stream.");

      sctr->min_id = UINT32_MAX;
      sctr->max_id = 0;

      sctr->process_manager = process_manager;
      sctr->pid = kl_reserve_process_id(process_manager,
         &_kl_scene_container_3d_process_tick,
         &_kl_scene_container_3d_advance_time, sctr);

      *container = sctr;
      ret = KL_SUCCESS;
   }

   return ret;
}

void kl_free_scene_container_3d(kl_scene_container_3d_t* container)
{
   kl_scene_container_3d_t sctr = *container;

   kl_release_process_id(sctr->process_manager, sctr->pid);
   kl_heap_free(sctr->reference);
   kl_heap_free(sctr->radius);
   kl_heap_free(sctr->typemask);
   kl_heap_free(sctr->transform);
   kl_free_idx_allocator(&sctr->id_allocator);
   kl_heap_free(sctr);

   *container = NULL;
}

int kl_reserve_scene_container_3d_id(kl_scene_container_3d_t container, uint32_t* out_id)
{
   float* xfm;
   uint32_t id = kl_idx_allocator_reserve(container->id_allocator);

   /* If there are no more free indices in the idx allocator, return error. */
   if(id == UINT32_MAX) return KL_ERROR;
   
   container->min_id = (id < container->min_id) ? id : container->min_id;
   container->max_id = (id > container->max_id) ? id : container->max_id;

   *out_id = id;
   xfm = &(container->transform[id * 16]);
   xfm[ 0] = 1.0f; xfm[ 1] = 0.0f; xfm[ 2] = 0.0f; xfm[ 3] = 0.0f;
   xfm[ 4] = 0.0f; xfm[ 5] = 1.0f; xfm[ 6] = 0.0f; xfm[ 7] = 0.0f;
   xfm[ 8] = 0.0f; xfm[ 9] = 0.0f; xfm[10] = 1.0f; xfm[11] = 0.0f;
   xfm[12] = 0.0f; xfm[13] = 0.0f; xfm[14] = 0.0f; xfm[15] = 1.0f;

   container->reference[id] = 0;
   container->typemask[id] = SceneType_Reserved;
   container->radius[id] = 1.0f;
   return KL_SUCCESS;
}

void kl_free_scene_container_3d_id(kl_scene_container_3d_t container, uint32_t id)
{
   container->typemask[id] = 0;
   kl_idx_allocator_release(container->id_allocator, id);
}

/* Internal callbacks for process manager */
void _kl_scene_container_3d_process_tick(void* context)
{
   kl_scene_container_3d_t sctr = (kl_scene_container_3d_t)context;
   KL_UNUSED(sctr);
}

void _kl_scene_container_3d_advance_time(float dt, void* context)
{
   kl_scene_container_3d_t sctr = (kl_scene_container_3d_t)context;
   KL_UNUSED(sctr);
}