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

#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/scriptinterface/types.h>
#include "scriptinterface/helpers/readers.h"

#define VECTOR3_ARRAY_INSTANCE_TABLE "vector3_array_instance_method_table"
const char* VECTOR3_ARRAY_LUA_LIB = "vector3_array";
int push_lua_vector3_array(lua_State* L, float* a, size_t sz);
int luaopen_vector3_array(lua_State* L);

extern int push_lua_vector3(lua_State* L, float* a);

int push_lua_vector3_array(lua_State* L, float* a, size_t sz)
{
   kl_lua_float_array_t* array = (kl_lua_float_array_t*)lua_newuserdata(L, sizeof(kl_lua_float_array_t));
   luaL_getmetatable(L, VECTOR3_ARRAY_LUA_LIB);
   lua_setmetatable(L, -2);

   array->len = sz;
   array->array = a;

   return 1;
}

static int vector3_array_index(lua_State* L)
{
   size_t len, idx;
   const char* key;
   kl_lua_float_array_t* array = (kl_lua_float_array_t*)lua_touserdata(L, 1);

   key = lua_tolstring(L, 2, &len);
   idx = atoi(key);
   if(idx > 0)
   {
      if(idx <= array->len)
      {
         return push_lua_vector3(L, &array->array[(idx - 1) * 3]);
      }
      else
      {
         lua_pushfstring(L, "Invalid index: '%s'.", key);
         lua_error(L); /* Never returns */
      }
   }

   lua_getglobal(L, VECTOR3_ARRAY_INSTANCE_TABLE);
   lua_pushvalue(L, 2);
   lua_gettable(L, -2);
   return lua_isnil(L, -1) ? 0 : 1;
}

static int vector3_array_newindex(lua_State* L)
{
   size_t len, idx;
   const char* key;
   kl_lua_float_array_t* array = (kl_lua_float_array_t*)lua_touserdata(L, 1);

   key = lua_tolstring(L, 2, &len);
   idx = atoi(key);

   if(idx > 0)
   {
      if(idx <= array->len)
      {
         lua_readvector3d(L, 3, &array->array[(idx - 1) * 3]);
         return 0;
      }
      else
      {
         lua_pushfstring(L, "Invalid index: '%s'.", key);
         lua_error(L); /* Never returns */
      }
   }


   lua_pushfstring(L, "Invalid index: '%s'.", key);
   lua_error(L); /* Never returns */
   return 0;
}

static int vector3_array_len(lua_State* L)
{
   kl_lua_float_array_t* array = (kl_lua_float_array_t*)lua_touserdata(L, 1);
   lua_pushinteger(L, array->len);
   return 1;
}

int luaopen_vector3_array(lua_State* L)
{
   luaL_newmetatable(L, VECTOR3_ARRAY_LUA_LIB);
   lua_pushcfunction(L, vector3_array_index);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, vector3_array_newindex);
   lua_setfield(L, -2, "__newindex");
   lua_pushcfunction(L, vector3_array_len);
   lua_setfield(L, -2, "__len");

   return 1;
}
