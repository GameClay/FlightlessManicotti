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

#include <FlightlessManicotti/beat/beat.h>
#include <FlightlessManicotti/core/timer.h>

kl_beat_manager_t g_beat_manager = NULL;

void _kl_beat_manager_advance_time(float dt, void* context);

int kl_beat_manager_alloc(kl_beat_manager_t* manager)
{
   int ret = KL_ERROR;
   if(manager != NULL)
   {
      kl_beat_manager_t mgr = kl_heap_alloc(sizeof(_kl_beat_manager));
      if(mgr != NULL)
      {
         kl_absolute_time_t current_time;
         mgr->beat_frequency_ns = 500 * 1e6; /* 120 bpm */
         kl_high_resolution_timer_query(&current_time);
         kl_absolute_time_to_ns(&current_time, &mgr->last_beat_time_ns);
         mgr->measure_count = 0;
         mgr->beat_count = 0;

         mgr->pid = kl_reserve_process_id(KL_DEFAULT_PROCESS_MANAGER,
            NULL, &_kl_beat_manager_advance_time, mgr);

         *manager = mgr;
         ret = KL_SUCCESS;
      }
   }
   return ret;
}

void kl_beat_manager_free(kl_beat_manager_t* manager)
{
   if(manager != NULL && *manager != NULL)
   {
      kl_release_process_id(KL_DEFAULT_PROCESS_MANAGER, (*manager)->pid);
      kl_heap_free(*manager);
   }
}

kl_beat_manager_t kl_beat_manager_default()
{
   return g_beat_manager;
}

void _kl_beat_manager_advance_time(float dt, void* context)
{
   kl_beat_manager_t mgr = (kl_beat_manager_t)context;
   kl_absolute_time_t current_time;
   uint64_t current_time_ns, beat_delta_ns, measure_delta_ns;
   uint64_t beat_count = mgr->beat_count;

   kl_high_resolution_timer_query(&current_time);
   kl_absolute_time_to_ns(&current_time, &current_time_ns);
   beat_delta_ns = current_time_ns - mgr->last_beat_time_ns;
   measure_delta_ns = current_time_ns - mgr->last_measure_time_ns;

   if(beat_delta_ns >= mgr->beat_frequency_ns)
   {
      mgr->last_beat_time_ns += mgr->beat_frequency_ns;
      beat_delta_ns = current_time_ns - mgr->last_beat_time_ns;
      if(beat_count % 4 == 0)
      {
         mgr->last_measure_time_ns = mgr->last_beat_time_ns;
         mgr->measure_count++;
      }
      measure_delta_ns = current_time_ns - mgr->last_measure_time_ns;
      mgr->beat_count = ++beat_count;
   }

   mgr->beat_interp = (float)beat_delta_ns / mgr->beat_frequency_ns;
   mgr->measure_interp = (float)measure_delta_ns / (mgr->beat_frequency_ns * 4);
}
