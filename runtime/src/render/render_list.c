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

#include <FlightlessManicotti/render/render_list.h>

int kl_render_list_init(kl_render_list_t* list, uint32_t list_sz)
{
   int ret = KL_ERROR;

   if(list != NULL)
   {
      ret = kl_alloc_idx_allocator(&list->idx_alloc, list_sz);
      list->max_idx = 0;
      list->list = kl_heap_alloc(sizeof(kl_render_instance_t*) * list_sz);
      kl_zero_mem(list->list, sizeof(kl_render_instance_t*) * list_sz);
   }

   return ret;
}

void kl_render_list_deinit(kl_render_list_t* list)
{
   if(list != NULL)
   {
      kl_heap_free(list->list);
      kl_free_idx_allocator(&list->idx_alloc);
   }
}

void kl_render_list_insert_instance(kl_render_list_t* list, kl_render_instance_t* inst)
{
   uint32_t idx = kl_idx_allocator_reserve(list->idx_alloc);
   list->list[idx] = inst;
   inst->list_index = idx;
   list->max_idx = (list->max_idx < idx ? idx : list->max_idx);
}

void kl_render_list_remove_instance(kl_render_list_t* list, kl_render_instance_t* inst)
{
   if(inst->list_index == UINT32_MAX)
   {
      uint32_t idx = inst->list_index;
      list->list[idx] = NULL;
      list->max_idx = (list->max_idx == idx ? idx - 1 : list->max_idx);
      kl_idx_allocator_release(list->idx_alloc, idx);
      inst->list_index = UINT32_MAX;
   }
}
