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

#include "getshader.h"
#include <string.h>
#include <stdio.h>
#include <lauxlib.h>

/* KL_DEFAULT_SCRIPT_CONTEXT */
extern kl_script_context_t g_script_context;

/* From: http://prideout.net/blog/?p=1 */
const char* GetShaderSource(kl_script_context_t context, const char* effectKey)
{
   lua_State* L;
   char effectName[32] = {0};
   const char* closestMatch = 0;
   int closestMatchLength = 0;
   int i;

   /* Extract the effect name: */
   const char* targetKey = strchr(effectKey, '.');
   if (!targetKey++)
      return NULL;

   strncpy(effectName, effectKey, targetKey - effectKey - 1);

   if(context == KL_DEFAULT_SCRIPT_CONTEXT)
      context = g_script_context;

   L = context->lua_state;

   /* Fetch the table from the Lua context and make sure it exists: */
   lua_getglobal(L, effectName);
   if (!lua_istable(L, -1))
   {
      char effectPath[64];

      lua_pop(L, 1);

      /* Delay-load the Lua file: */
      sprintf(effectPath, "%s.lua", effectName);
      if (luaL_dofile(L, effectPath))
         return NULL;

      /* If it's still not there, give up! */
      lua_getglobal(L, effectName);
      if (!lua_istable(L, -1))
         return NULL;
   }

   i = lua_gettop(L);
   lua_pushnil(L);
   while (lua_next(L, i) != 0)
   {
      const char* shaderKey = lua_tostring(L, -2);
      int shaderKeyLength = strlen(shaderKey);

      /* Find the longest key that matches the beginning of the target key: */
      if (strstr(targetKey, shaderKey) != 0 && shaderKeyLength > closestMatchLength)
      {
         closestMatchLength = shaderKeyLength;
         closestMatch = lua_tostring(L, -1);
      }

      lua_pop(L, 1);
   }

   lua_pop(L, 1);

   return closestMatch;
}
