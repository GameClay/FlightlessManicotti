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
#include <FlightlessManicotti/render/render_list.h>

extern const char* VECTOR2D_LUA_LIB;
extern const char* VECTOR3D_LUA_LIB;
extern const char* RENDER_LIST_LUA_LIB;

extern kl_render_context_t g_script_render_context;

static int Manicotti_noise(lua_State* L)
{
   if(lua_isnumber(L, 1))
   {
      if(lua_isnumber(L, 2))
      {
         if(lua_isnumber(L, 3))
         {
            if(lua_isnumber(L, 4))
            {
               lua_pushnumber(L, kl_simplex_noise_4d(
                  (float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                  (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4)));
            }
            else
            {
               lua_pushnumber(L, kl_simplex_noise_3d(
                  (float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                  (float)lua_tonumber(L, 3)));
            }
         }
         else
         {
            lua_pushnumber(L, kl_simplex_noise_2d((float)lua_tonumber(L, 1),
               (float)lua_tonumber(L, 2)));
         }
      }
      else
      {
         lua_pushnumber(L, kl_simplex_noise_1d((float)lua_tonumber(L, 1)));
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
      luaL_argcheck(L, 0, 1, "expected 1-4 number(s), table, vector2d or vector3d");
   }

   return 1;
}

static int Manicotti_setrenderlist(lua_State* L)
{
   kl_render_list_t* list = NULL;
   int list_idx = 0;

   if(!lua_isnoneornil(L, 1))
   {
      list = (kl_render_list_t*)luaL_checkudata(L, 1, RENDER_LIST_LUA_LIB);
   }

   if(!lua_isnoneornil(L, 2))
   {
      luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected nil, or render list index to assign");
      list_idx = (int)lua_tointeger(L, 2);
   }

   if(kl_render_assign_list(g_script_render_context, list, list_idx) != KL_TRUE)
   {
      lua_pushstring(L, "render list index out of range");
      lua_error(L);
   }

   return 0;
}

static const struct luaL_reg manicotti_module [] = {
    {"noise", Manicotti_noise},
    {"setrenderlist", Manicotti_setrenderlist},
    {NULL, NULL}
};

int luaopen_manicotti(lua_State* L)
{
   luaL_register(L, "Manicotti", manicotti_module);
   return 1;
}
