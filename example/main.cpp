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
#include "script/script.h"
#include "core/error_codes.h"

int main(int argc, const char* argv[])
{
   printf("FlightlessManicotti\n");
   
   for(int i = 0; i < argc; i++)
   {
      printf("\targ %d: %s\n", i, argv[i]);
   }
   
   gc_script_context script_context;
   if(gc_script_init(&script_context, 1 << 10) == GC_SUCCESS)
   {
      gc_script_run(script_context, "example/script.lua", true, argc, argv);

      gc_script_destroy(&script_context);
   }
   
   return 0;
}