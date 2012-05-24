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
 
#include <stdio.h>
#include <stdlib.h>
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/scriptinterface/script.h>

int main(int argc, const char* argv[])
{
   if(kl_initialize(KL_FALSE, KL_TRUE, "example/main.lua", argc, argv) == KL_SUCCESS)
   {
      // Send the script a test event
      kl_script_event_t fooevt;
      fooevt.event.id = kl_register_script_event("TestEvent");
      fooevt.event.context.as_ptr = NULL;
      fooevt.event.arg.as_uint32 = 42;
      
      kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &fooevt);
      
      while(kl_mainloop_iteration() == KL_SUCCESS)
         ;
      
      kl_destroy();
   }
#ifdef WIN32
   printf("Press any key to continue...");
   getchar();
#endif
   
   return 0;
}