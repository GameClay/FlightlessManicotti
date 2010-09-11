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

#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "scriptinterface/script.h"

static int kl_script_event_dequeue_wrap(lua_State* L)
{
   kl_script_context_t sctx = (kl_script_context_t)lua_topointer(L, 1);
   
   kl_script_event_t event;
   if(kl_script_event_dequeue(sctx, &event) == KL_SUCCESS)
   {
      lua_pushstring(L, event.name);
      lua_pushlightuserdata(L, event.context);
      lua_pushinteger(L, event.a);
      lua_pushinteger(L, event.b);
      lua_pushinteger(L, event.c);
      return 5;
   }
   
   lua_pushnil(L);
   return 1;
}

static int kl_script_frame_done(lua_State* L)
{
   kl_script_event_fence_t* fence;
   kl_script_context_t sctx;
   
   sctx = (kl_script_context_t)lua_topointer(L, 1);
   fence = (kl_script_event_fence_t*)lua_topointer(L, 2);
   if(fence != NULL)
      kl_script_event_fence_notify(fence);
   
   return 0;
}

static const struct luaL_reg scriptevent_module [] = {
    {"dequeue", kl_script_event_dequeue_wrap},
    {"framedone", kl_script_frame_done},
    {NULL, NULL}
};

int luaopen_scriptevents(lua_State* L)
{
   luaL_register(L, "script.events", scriptevent_module);
   return 1;
}