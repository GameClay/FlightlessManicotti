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
#include "scriptinterface/helpers/vector.h"

#define VECTOR3D_INSTANCE_TABLE "vector3d_instance_method_table"
const char* VECTOR3D_LUA_LIB = "vector3d";

static int vector3d_index(lua_State* L)
{
   size_t len;
   const char* key;
   float* xyz = (float*)lua_topointer(L, 1);

   /* Check for x, yor z, else redirect to instance table */
   key = lua_tolstring(L, 2, &len);

   if(len == 1)
   {
      switch(*key)
      {
         case 'x':
         {
            lua_pushnumber(L, xyz[0]);
            return 1;
         }
         case 'y':
         {
            lua_pushnumber(L, xyz[1]);
            return 1;
         }
         case 'z':
         {
            lua_pushnumber(L, xyz[2]);
         }
      }
   }
   else if(len == 2 && key[0] == 'x' && key[1] == 'y')
   {
      lua_pushnumber(L, xyz[0]);
      lua_pushnumber(L, xyz[1]);
      return 2;
   }
   else if(len == 3 && key[0] == 'x' && key[1] == 'y' && key[2] == 'z')
   {
      lua_pushnumber(L, xyz[0]);
      lua_pushnumber(L, xyz[1]);
      lua_pushnumber(L, xyz[2]);
      return 2;
   }

   /* Wasn't x or y, so check the instance table */
   lua_getglobal(L, VECTOR3D_INSTANCE_TABLE);
   lua_pushvalue(L, 2);
   lua_gettable(L, -2);
   return lua_isnil(L, -1) ? 0 : 1;
}

static int vector3d_newindex(lua_State* L)
{
   size_t len;
   const char* key;
   float* xyz = (float*)lua_topointer(L, 1);

   /* Check for x, y or z, else redirect to instance table */
   key = lua_tolstring(L, 2, &len);
   if(len == 1)
   {
      switch(*key)
      {
         case 'x':
         {
            luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number");
            xyz[0] = lua_tonumber(L, 3);
            return 0;
         }
         case 'y':
         {
            luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number");
            xyz[1] = lua_tonumber(L, 3);
            return 0;
         }
         case 'z':
         {
            luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number");
            xyz[2] = lua_tonumber(L, 3);
            return 0;
         }
      }
   }
   else if(len == 2 && key[0] == 'x' && key[1] == 'y')
   {
      lua_readvector2d(L, 3, xyz);
      return 0;
   }
   else if(len == 3 && key[0] == 'x' && key[1] == 'y' && key[2] == 'z')
   {
      lua_readvector3d(L, 3, xyz);
      return 0;
   }

   /* Wasn't x or y, invalid */
   lua_pushfstring(L, "Invalid field: '%s'.", key);
   lua_error(L); /* Never returns */
   return 0;
}

static int vector3d_tostring(lua_State* L)
{
   float* xyz = (float*)lua_topointer(L, 1);
   lua_pushfstring(L, "(%f, %f, %f)", xyz[0], xyz[1], xyz[2]);
   return 1;
}

static const struct luaL_reg vector3d_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg vector3d_class_methods [] = {
   {NULL, NULL}
};

int luaopen_vector3d(lua_State* L)
{
   luaL_register(L, VECTOR3D_INSTANCE_TABLE, vector3d_instance_methods);

   luaL_newmetatable(L, VECTOR3D_LUA_LIB);
   lua_pushcfunction(L, vector3d_index);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, vector3d_newindex);
   lua_setfield(L, -2, "__newindex");
   lua_pushcfunction(L, vector3d_tostring);
   lua_setfield(L, -2, "__tostring");

   luaL_register(L, VECTOR3D_LUA_LIB, vector3d_class_methods);

   return 1;
}
