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
#include <FlightlessManicotti/math/noise.h>

extern const char* VECTOR2D_LUA_LIB;
extern const char* VECTOR3D_LUA_LIB;

static int vizthingy_noise(lua_State* L)
{
   if(lua_isnumber(L, 1))
   {
      if(lua_isnumber(L, 2))
      {
         if(lua_isnumber(L, 3))
         {
            if(lua_isnumber(L, 4))
            {
               lua_pushnumber(L, kl_simplex_noise_4d(lua_tonumber(L, 1), lua_tonumber(L, 2),
                  lua_tonumber(L, 3), lua_tonumber(L, 4)));
            }
            else
            {
               lua_pushnumber(L, kl_simplex_noise_3d(lua_tonumber(L, 1), lua_tonumber(L, 2),
                  lua_tonumber(L, 3)));
            }
         }
         else
         {
            lua_pushnumber(L, kl_simplex_noise_2d(lua_tonumber(L, 1), lua_tonumber(L, 2)));
         }
      }
      else
      {
         lua_pushnumber(L, kl_simplex_noise_1d(lua_tonumber(L, 1)));
      }
   }
   else if(lua_istable(L, 1))
   {

   }
   else if(luaL_checkudata(L, 1, VECTOR2D_LUA_LIB) != NULL)
   {

   }
   else if(luaL_checkudata(L, 1, VECTOR3D_LUA_LIB) != NULL)
   {

   }
   else
   {
      luaL_argcheck(L, 0, 1, "expected number(s), table, vector2d or vector3d");
   }

   return 1;
}

static const struct luaL_reg vizthingy_module [] = {
    {"noise", vizthingy_noise},
    {NULL, NULL}
};

int luaopen_vizthingy(lua_State* L)
{
   luaL_register(L, "vizthingy", vizthingy_module);
   return 1;
}
