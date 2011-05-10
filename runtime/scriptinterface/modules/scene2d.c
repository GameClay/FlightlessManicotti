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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "game/scene2d/scene_container_2d.h"

#define SCENE2D_LIB "Scene2D"

static int Scene2D_new(lua_State* L)
{
   uint32_t max_entries;
   kl_scene_container_2d_t* sctr;
   luaL_checkinteger(L, 1);
   
   sctr = (kl_scene_container_2d_t*)lua_newuserdata(L, sizeof(kl_scene_container_2d_t));
   max_entries = lua_tointeger(L, 1);
   
   if(kl_alloc_scene_container_2d(sctr, KL_DEFAULT_PROCESS_MANAGER, max_entries) == KL_SUCCESS)
   {
      luaL_getmetatable(L, SCENE2D_LIB);
      lua_setmetatable(L, -2);
   }
   else
   {
      lua_pushnil(L);
      luaL_error(L, "Creation of Scene2D failed: kl_alloc_scene_container_2d was not successful.");
   }
   
   // The userdata (kl_scene_container_2d) was put on the stack by the 'lua_newuserdata' call
   return 1;
}

static int Scene2D_gc(lua_State *L)
{
   kl_scene_container_2d_t* sctr = (kl_scene_container_2d_t*)lua_touserdata(L, 1);
   if(sctr != NULL) kl_free_scene_container_2d(sctr);
   return 0;
}

static int Scene2D_reserveid(lua_State *L)
{
   kl_scene_container_2d_t* sctr;
   luaL_checkudata(L, 1, SCENE2D_LIB);
   
   sctr = (kl_scene_container_2d_t*)lua_touserdata(L, 1);
   if(sctr != NULL)  lua_pushinteger(L, kl_reserve_scene_container_2d_id(*sctr));
   else              lua_pushnil(L);
   
   return 1;
}

static int Scene2D_releaseid(lua_State *L)
{
   int scene_id;
   kl_scene_container_2d_t* sctr;
   
   luaL_checkudata(L, 1, SCENE2D_LIB);
   luaL_argcheck(L, lua_type(L, 2) == LUA_TNUMBER, 2, "Expected scene id as argument");
   
   sctr = (kl_scene_container_2d_t*)lua_touserdata(L, 1);
   scene_id = lua_tointeger(L, 2);
   
   if(sctr != NULL) kl_free_scene_container_2d_id(*sctr, scene_id);
   return 0;
}

static const struct luaL_reg Scene2D_instance_methods [] = {
   {"reserveid", Scene2D_reserveid},
   {"releaseid", Scene2D_releaseid},
   {NULL, NULL}
};

static const struct luaL_reg Scene2D_class_methods [] = {
   {"new", Scene2D_new},
   {NULL, NULL}
};

int luaopen_scene2d(lua_State* L)
{
   luaL_newmetatable(L, SCENE2D_LIB);
   luaL_register(L, 0, Scene2D_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   
   lua_pushcfunction(L, Scene2D_gc);
   lua_setfield(L, -2, "__gc");
   
   luaL_register(L, SCENE2D_LIB, Scene2D_class_methods);
   
   return 1;
}