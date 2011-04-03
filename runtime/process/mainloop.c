/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2009-2011 GameClay LLC
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
#include "scriptinterface/script.h"
#include "process/process.h"

kl_script_event_fence_t scriptfence;
KL_BOOL pump_script;

int kl_init_mainloop(const char* main_script, int argc, const char* argv[])
{
   int ret = KL_SUCCESS;
   
   //////////////////////////////////
   // Get the script processing ready
   //////////////////////////////////
   
   // Determine if we need to pump the script processor, or if it is asynchronous
   pump_script = (kl_script_is_threaded(KL_DEFAULT_SCRIPT_CONTEXT) == KL_FALSE);
   
   // If main script fails to load, bail.
   if(kl_script_run(KL_DEFAULT_SCRIPT_CONTEXT, main_script, argc, argv) != KL_SUCCESS)
   {
      ret = KL_ERROR;
      KL_ASSERT(KL_FALSE, "Main script execution failed.");
   }
   
   // Prime the script-event queue.
   kl_script_event_endframe(KL_DEFAULT_SCRIPT_CONTEXT, &scriptfence);
   if(pump_script && (kl_script_event_pump(KL_DEFAULT_SCRIPT_CONTEXT) != KL_SUCCESS))
   {
      ret = KL_ERROR;
      KL_ASSERT(KL_FALSE, "Script-event handler execution failed.");
   }
   kl_script_event_endframe(KL_DEFAULT_SCRIPT_CONTEXT, NULL);
   
   return ret;
}

int kl_mainloop_iteration(const char* main_script, int argc, const char* argv[])
{
   int ret = KL_SUCCESS;
   
   
   //////////////////////
   // Update packet frame
   //////////////////////
   
   // ...
   
   //////////////////////
   // Update script frame
   //////////////////////
   
   // Pump the script-event queue if this is unthreaded script
   if(pump_script)
   {
      if(kl_script_event_pump(KL_DEFAULT_SCRIPT_CONTEXT) != KL_SUCCESS)
         return KL_ERROR;
   }
   else
   {
      // Wait on fence from previous frame
      while(kl_script_event_fence_wait(&scriptfence) == KL_RETRY)
         ;
   }
   
   //////////////////////////
   // Update simulation frame
   //////////////////////////
   
   // TODO: If tick-time has past
   kl_tick_process_object_list(KL_DEFAULT_PROCESS_OBJECT_MANAGER);
   
   kl_advance_process_object_list(KL_DEFAULT_PROCESS_OBJECT_MANAGER, 0.0f);
   
   ////////////
   // Do output
   ////////////
   
   // ...
   
   ////////////////////////////
   // Runtime frame is complete
   ////////////////////////////
   
   // Send endframe to script-event queue.
   kl_script_event_endframe(KL_DEFAULT_SCRIPT_CONTEXT, &scriptfence);
   
   return ret;
}