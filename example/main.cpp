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
 
#include <stdio.h>
#include <stdlib.h>
#include "fm.h"
#include "scriptinterface/script.h"

int main(int argc, const char* argv[])
{
   if(kl_initialize(KL_TRUE) == KL_SUCCESS)
   {
      // Send the script a test event
      kl_script_event_t fooevt = {"facepunch", NULL, 0, 1, 2};
      kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &fooevt);
      
      kl_mainloop("example/main.lua", argc, argv);
      
      // Dequeue events from script
      while(kl_script_event_dequeue(KL_DEFAULT_SCRIPT_CONTEXT, &fooevt) == KL_SUCCESS)
      {
         printf("From script: {%s,%p,%d,%d,%d}\n", fooevt.name, fooevt.context,
            fooevt.a, fooevt.b, fooevt.c);
      }
      
      kl_destroy();
   }
#ifdef WIN32
   printf("Press any key to continue...");
   getchar();
#endif
   
   return 0;
}