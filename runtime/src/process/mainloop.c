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

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/scriptinterface/script.h>
#include <FlightlessManicotti/process/process.h>
#include <FlightlessManicotti/core/timer.h>

kl_script_event_fence_t* g_mainloop_scriptfence = NULL;
KL_BOOL pump_script;
kl_absolute_time_t g_mainloop_last_frame_time = 0;
kl_absolute_time_t g_mainloop_last_tick_time = 0;
kl_absolute_time_t g_mainloop_elapsed_time = 0;
kl_absolute_time_t g_mainloop_tick_frequency;

int kl_init_mainloop(const char* main_script, KL_BOOL wait_on_fences, int argc, const char* argv[])
{
   int ret = KL_SUCCESS;
   uint64_t tick_frequency_ns = 32 * 1e6;

   /*
    * Get the script processing ready
    */

   /* Assign script fence if requested */
   if(wait_on_fences == KL_TRUE)
   {
      g_mainloop_scriptfence = kl_heap_alloc(sizeof(kl_script_event_fence_t));
      kl_zero_mem(g_mainloop_scriptfence, sizeof(kl_script_event_fence_t));
   }

   /* Determine if we need to pump the script processor, or if it is asynchronous */
   pump_script = (kl_script_is_threaded(KL_DEFAULT_SCRIPT_CONTEXT) == KL_FALSE);

   /* If main script fails to load, bail. */
   if(kl_script_run(KL_DEFAULT_SCRIPT_CONTEXT, main_script, argc, argv) != KL_SUCCESS)
   {
      ret = KL_ERROR;
      KL_ASSERT(KL_FALSE, "Main script execution failed.");
   }

   /* Prime the script-event queue. */
   kl_script_event_endframe(KL_DEFAULT_SCRIPT_CONTEXT, g_mainloop_scriptfence);
   if(pump_script && (kl_script_event_pump(KL_DEFAULT_SCRIPT_CONTEXT) != KL_SUCCESS))
   {
      ret = KL_ERROR;
      KL_ASSERT(KL_FALSE, "Script-event handler execution failed.");
   }
   kl_script_event_endframe(KL_DEFAULT_SCRIPT_CONTEXT, NULL);

   /* Compute tick frequency in absolute-time */
   kl_ns_to_absolute_time(&tick_frequency_ns, &g_mainloop_tick_frequency);
   
   return ret;
}

int kl_mainloop_iteration()
{
   int ret = KL_SUCCESS;

   kl_absolute_time_t frame_timestamp;
   kl_absolute_time_t delta_time;
   uint64_t delta_ns;
   float dt;

   /* Initialize last frame time to now */
   if(g_mainloop_last_frame_time == 0)
   {
      kl_high_resolution_timer_query(&g_mainloop_last_frame_time);
      g_mainloop_last_tick_time = g_mainloop_last_frame_time;
   }

   /*
    * Prepare for frame
    */

   /* Get a delta time since last frame */
   kl_high_resolution_timer_query(&frame_timestamp);
   delta_time = frame_timestamp - g_mainloop_last_frame_time;

   kl_absolute_time_to_ns(&delta_time, &delta_ns);
   dt = (float)delta_ns * 1e-6; /* Convert to ms */

   /*
    * Update packet frame
    */

   /* ... */

   /*
    * Update script frame
    */

   /* Pump the script-event queue if this is unthreaded script */
   if(pump_script)
   {
      if(kl_script_event_pump(KL_DEFAULT_SCRIPT_CONTEXT) != KL_SUCCESS)
         return KL_ERROR;
   }
   else if(g_mainloop_scriptfence != NULL)
   {
      /* Wait on fence from previous frame */
      while(kl_script_event_fence_wait(g_mainloop_scriptfence) == KL_RETRY)
         ;
   }

   /*
    * Update simulation frame
    */

   while(g_mainloop_last_tick_time + g_mainloop_tick_frequency < frame_timestamp)
   {
      /* Tell default process manager to tick */
      kl_tick_process_list(KL_DEFAULT_PROCESS_MANAGER);

      /* Send script tick event */
      kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &g_tick_script_event);
      g_mainloop_last_tick_time += g_mainloop_tick_frequency;
   }

   /* Tell default process manager to advance-time */
   kl_advance_process_list(KL_DEFAULT_PROCESS_MANAGER, dt);

   /* Send script advance-time event */
   g_advance_time_script_event.event.arg.as_float = dt;
   kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &g_advance_time_script_event);

   /*
    * End script event frame
    */
   kl_script_event_endframe(KL_DEFAULT_SCRIPT_CONTEXT, g_mainloop_scriptfence);

   /*
    * Runtime frame is complete
    */

   /* Record end time of this frame */
   kl_high_resolution_timer_query(&g_mainloop_last_frame_time);

   /* Get how long it took this frame to execute */
   delta_time = g_mainloop_last_frame_time - frame_timestamp;
   kl_absolute_time_to_ns(&delta_time, &delta_ns);
   dt = (float)delta_ns * 1e-6;

   /* Increment elapsed time */
   g_mainloop_elapsed_time += delta_time;

   return ret;
}
