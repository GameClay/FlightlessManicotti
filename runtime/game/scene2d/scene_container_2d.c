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
#include "process/process.h"

struct _kl_scene_container_2d
{
   uint32_t pid;
};

int kl_alloc_scene_container_2d(kl_scene_container_2d_t* container)
{
   int ret = KL_ERROR;
   kl_scene_container_2d_t sctr;
   
   KL_ASSERT(container != NULL, "NULL manager pointer.");
   sctr = kl_heap_alloc(sizeof(struct _kl_scene_container_2d));
   
   if(sctr != NULL)
   {
      sctr->pid = kl_reserve_process_object_id(KL_DEFAULT_PROCESS_OBJECT_MANAGER, NULL, NULL, sctr);
      ret = KL_SUCCESS;
      *container = sctr;
   }
   return ret;
}

void kl_free_scene_container_2d(kl_scene_container_2d_t* container)
{
   kl_scene_container_2d_t sctr = *container;
   kl_release_process_object_id(KL_DEFAULT_PROCESS_OBJECT_MANAGER, sctr->pid);
   kl_heap_free(sctr);
   *container = NULL;
}