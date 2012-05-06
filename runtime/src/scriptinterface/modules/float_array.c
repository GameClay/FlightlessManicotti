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
#include <FlightlessManicotti/fm.h>

#define FLOAT_ARRAY_INSTANCE_TABLE "float_array_instance_method_table"
const char* FLOAT_ARRAY_LUA_LIB = "float_array";

static int float_array_index(lua_State* L)
{
   size_t len;
   const char* key;
   int idx;
   float* array = (float*)lua_topointer(L, 1);

   key = lua_tolstring(L, 2, &len);
   idx = atoi(key);
   if(idx > 0)
   {
      lua_pushnumber(L, array[idx - 1]);
      return 1;
   }

   lua_getglobal(L, FLOAT_ARRAY_INSTANCE_TABLE);
   lua_pushvalue(L, 2);
   lua_gettable(L, -2);
   return lua_isnil(L, -1) ? 0 : 1;
}

static int float_array_newindex(lua_State* L)
{
   size_t len;
   int idx;
   const char* key;
   float* array = (float*)lua_topointer(L, 1);

   key = lua_tolstring(L, 2, &len);
   idx = atoi(key);

   if(idx > 0)
   {
      luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number");
      array[idx - 1] = lua_tonumber(L, 3);
      return 0;
   }

   lua_pushfstring(L, "Invalid index: '%s'.", key);
   lua_error(L); /* Never returns */
   return 0;
}

int luaopen_float_array(lua_State* L)
{
   luaL_newmetatable(L, FLOAT_ARRAY_LUA_LIB);
   lua_pushcfunction(L, float_array_index);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, float_array_newindex);
   lua_setfield(L, -2, "__newindex");

   return 1;
}
