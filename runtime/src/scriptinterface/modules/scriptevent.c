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

#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <FlightlessManicotti/scriptinterface/script.h>
#include <FlightlessManicotti/scriptinterface/scriptevents.h>
#include <FlightlessManicotti/scriptinterface/types.h>

static int kl_script_event_dequeue_wrap(lua_State* L)
{
   kl_script_context_t sctx = (kl_script_context_t)lua_topointer(L, 1);
   
   kl_script_event_t event;
   if(kl_script_event_dequeue(sctx, &event) == KL_SUCCESS)
   {
      uint32_t context_type = kl_get_script_event_context_type(event.event.id);
      int ret = 3;
      lua_pushinteger(L, event.event.id);

      if(context_type == KL_SCRIPT_CONTEXT_TYPE_ASSIGNER)
      {
         kl_script_event_context_assigner_fn assigner =
            kl_get_script_event_context_assigner(event.event.id);
         ret = assigner(L, &event) + 1;
      }
      else
      {
         switch(context_type)
         {
            case 0:
            {
               lua_pushlightuserdata(L, event.event.context.as_ptr);
               break;
            }

            case LUA_TNUMBER:
            {
               lua_pushnumber(L, *((lua_Number*)event.event.context.as_ptr));
               kl_heap_free(event.event.context.as_ptr);
               break;
            }

            case LUA_TSTRING:
            {
               lua_pushstring(L, event.event.context.as_ptr);
               kl_heap_free(event.event.context.as_ptr);
               break;
            }

         }
         lua_pushinteger(L, event.event.arg.as_uint32);
      }
      return ret;
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

static int kl_register_script_event_wrap(lua_State* L)
{
   kl_script_context_t sctx = (kl_script_context_t)lua_topointer(L, 1);
   const char* name = lua_tostring(L, 2);
   KL_UNUSED(sctx);
   
   lua_pushinteger(L, kl_register_script_event(name));
   return 1;
}

static const struct luaL_reg scriptevent_module [] = {
    {"dequeue", kl_script_event_dequeue_wrap},
    {"framedone", kl_script_frame_done},
    {"register", kl_register_script_event_wrap},
    {NULL, NULL}
};

int luaopen_scriptevents(lua_State* L)
{
   luaL_register(L, "Events", scriptevent_module);
   return 1;
}
