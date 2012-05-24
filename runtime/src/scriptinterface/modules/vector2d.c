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
#include <FlightlessManicotti/scriptinterface/types.h>
#include "scriptinterface/helpers/readers.h"

#define VECTOR2D_INSTANCE_TABLE "vector2d_instance_method_table"
const char* VECTOR2D_LUA_LIB = "vector2d";

int push_lua_vector2(lua_State* L, float* a)
{
   lua_vectorf* vec = (lua_vectorf*)lua_newuserdata(L, sizeof(lua_vectorf));
   luaL_getmetatable(L, VECTOR2D_LUA_LIB);
   lua_setmetatable(L, -2);

   vec->vec = a;

   return 1;
}

static int vector2d_index(lua_State* L)
{
   size_t len;
   const char* key;
   lua_vectorf* vec = (lua_vectorf*)lua_topointer(L, 1);
   float* xy = vec->vec;

   /* Check for x or y, else redirect to instance table */
   key = lua_tolstring(L, 2, &len);

   if(len == 1)
   {
      switch(*key)
      {
         case 'x':
         {
            lua_pushnumber(L, xy[0]);
            return 1;
         }
         case 'y':
         {
            lua_pushnumber(L, xy[1]);
            return 1;
         }
      }
   }
   else if(len == 2 && key[0] == 'x' && key[1] == 'y')
   {
      lua_pushnumber(L, xy[0]);
      lua_pushnumber(L, xy[1]);
      return 2;
   }

   /* Wasn't x or y, so check the instance table */
   lua_getglobal(L, VECTOR2D_INSTANCE_TABLE);
   lua_pushvalue(L, 2);
   lua_gettable(L, -2);
   return lua_isnil(L, -1) ? 0 : 1;
}

static int vector2d_newindex(lua_State* L)
{
   size_t len;
   const char* key;
   lua_vectorf* vec = (lua_vectorf*)lua_topointer(L, 1);
   float* xy = vec->vec;

   /* Check for x or y, else redirect to instance table */
   key = lua_tolstring(L, 2, &len);
   if(len == 1)
   {
      switch(*key)
      {
         case 'x':
         {
            luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number");
            xy[0] = lua_tonumber(L, 3);
            return 0;
         }
         case 'y':
         {
            luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number");
            xy[1] = lua_tonumber(L, 3);
            return 0;
         }
      }
   }
   else if(len == 2 && key[0] == 'x' && key[1] == 'y')
   {
      lua_readvector2d(L, 3, xy);
      return 0;
   }

   /* Wasn't x or y, invalid */
   lua_pushfstring(L, "Invalid field: '%s'.", key);
   lua_error(L); /* Never returns */
   return 0;
}

static int vector2d_tostring(lua_State* L)
{
   lua_vectorf* vec = (lua_vectorf*)lua_topointer(L, 1);
   float* xy = vec->vec;
   lua_pushfstring(L, "(%f, %f)", xy[0], xy[1]);
   return 1;
}

static const struct luaL_reg vector2d_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg vector2d_class_methods [] = {
   {NULL, NULL}
};

int luaopen_vector2d(lua_State* L)
{
   luaL_register(L, VECTOR2D_INSTANCE_TABLE, vector2d_instance_methods);

   luaL_newmetatable(L, VECTOR2D_LUA_LIB);
   lua_pushcfunction(L, vector2d_index);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, vector2d_newindex);
   lua_setfield(L, -2, "__newindex");
   lua_pushcfunction(L, vector2d_tostring);
   lua_setfield(L, -2, "__tostring");

   luaL_register(L, VECTOR2D_LUA_LIB, vector2d_class_methods);

   return 1;
}
