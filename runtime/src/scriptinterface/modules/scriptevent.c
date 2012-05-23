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

#include <FlightlessManicotti/beat/cqt_wave.h>

extern const char* FLOAT_ARRAY_LUA_LIB;
extern const char* CQT_WAVE_ARRAY_LUA_LIB;

static int kl_script_event_dequeue_wrap(lua_State* L)
{
   kl_script_context_t sctx = (kl_script_context_t)lua_topointer(L, 1);
   
   kl_script_event_t event;
   if(kl_script_event_dequeue(sctx, &event) == KL_SUCCESS)
   {
      uint32_t context_type = kl_get_script_event_context_type(event.event.id);
      lua_pushinteger(L, event.event.id);

      switch(context_type)
      {
         case 0:
         {
            lua_pushlightuserdata(L, event.event.context.as_ptr);
            break;
         }

         case LUA_TSTRING:
         {
            lua_pushstring(L, event.event.context.as_ptr);
            kl_heap_free(event.event.context.as_ptr);
            break;
         }

         case KL_SCRIPT_EVENT_CONTEXT_TYPE_FLOAT_ARRAY:
         {
            kl_lua_float_array_t* array = (kl_lua_float_array_t*)lua_newuserdata(L, sizeof(kl_lua_float_array_t));
            memcpy(array, event.event.context.as_ptr, sizeof(kl_lua_float_array_t));
            luaL_getmetatable(L, FLOAT_ARRAY_LUA_LIB);
            lua_setmetatable(L, -2);
            break;
         }

         case KL_SCRIPT_EVENT_CONTEXT_TYPE_CQT_WAVE_ARRAY:
         {
            kl_lua_cqt_wave_array_t* array = (kl_lua_cqt_wave_array_t*)lua_newuserdata(L, sizeof(kl_lua_cqt_wave_array_t));
            memcpy(array, event.event.context.as_ptr, sizeof(kl_lua_cqt_wave_array_t));
            luaL_getmetatable(L, CQT_WAVE_ARRAY_LUA_LIB);
            lua_setmetatable(L, -2);
            break;
         }
      }
      lua_pushinteger(L, event.event.arg);
      return 3;
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