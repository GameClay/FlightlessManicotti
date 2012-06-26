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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/scriptinterface/types.h>
#include "scriptinterface/helpers/readers.h"

extern const char* VECTOR2D_LUA_LIB;
extern const char* VECTOR3D_LUA_LIB;

void lua_readvector2d(lua_State* L, int param_idx, float* out_xy)
{
   if(lua_istable(L, param_idx))
   {
      int top = lua_gettop(L);
      lua_pushinteger(L, 1);
      lua_gettable(L, param_idx - 1);
      lua_pushinteger(L, 2);
      lua_gettable(L, param_idx - 2);
      luaL_argcheck(L, lua_isnumber(L, top + 1) && lua_isnumber(L, top + 2), 
         param_idx - 2, "expected numerical array");
      out_xy[0] = (float)lua_tonumber(L, top + 1);
      out_xy[1] = (float)lua_tonumber(L, top + 2);
      lua_pop(L, 2);
   }
   else if(lua_isuserdata(L, 3))
   {
      /* Won't return if error */
      lua_vectorf* vec = (lua_vectorf*)luaL_checkudata(L, param_idx, VECTOR2D_LUA_LIB);
      float* tempxy = vec->vec;
      out_xy[0] = tempxy[0];
      out_xy[1] = tempxy[1];
   }
   else
   {
      luaL_argcheck(L, 0, 3, "expected numerical array, or vector2d");
   }
}

void lua_readvector3d(lua_State* L, int param_idx, float* out_xyz)
{
   if(lua_istable(L, param_idx))
   {
      int top = lua_gettop(L);
      lua_pushinteger(L, 1);
      lua_gettable(L, param_idx - 1);
      lua_pushinteger(L, 2);
      lua_gettable(L, param_idx - 2);
      lua_pushinteger(L, 3);
      lua_gettable(L, param_idx - 3);
      luaL_argcheck(L, lua_isnumber(L, top + 1) && lua_isnumber(L, top + 2) && lua_isnumber(L, top + 3),
         param_idx - 3, "expected numerical array");
      out_xyz[0] = (float)lua_tonumber(L, top + 1);
      out_xyz[1] = (float)lua_tonumber(L, top + 2);
      out_xyz[2] = (float)lua_tonumber(L, top + 3);
      lua_pop(L, 3);
   }
   else if(lua_isuserdata(L, 3))
   {
      /* Won't return if error */
      lua_vectorf* vec = (lua_vectorf*)luaL_checkudata(L, param_idx, VECTOR3D_LUA_LIB);
      float* tempxyz = vec->vec;
      out_xyz[0] = tempxyz[0];
      out_xyz[1] = tempxyz[1];
      out_xyz[2] = tempxyz[2];
   }
   else
   {
      luaL_argcheck(L, 0, 3, "expected numerical array, or vector3d");
   }
}

void lua_readtriangleface(lua_State* L, int param_idx, uint16_t* out_abc)
{
   if(lua_istable(L, param_idx))
   {
      int top = lua_gettop(L);
      lua_pushinteger(L, 1);
      lua_gettable(L, param_idx - 1);
      lua_pushinteger(L, 2);
      lua_gettable(L, param_idx - 2);
      lua_pushinteger(L, 3);
      lua_gettable(L, param_idx - 3);
      luaL_argcheck(L, lua_isnumber(L, top + 1) && lua_isnumber(L, top + 2) && lua_isnumber(L, top + 3),
         param_idx - 3, "expected numerical array");
      out_abc[0] = (uint16_t)lua_tointeger(L, top + 1);
      out_abc[1] = (uint16_t)lua_tointeger(L, top + 2);
      out_abc[2] = (uint16_t)lua_tointeger(L, top + 3);
      lua_pop(L, 3);
   }
   else
   {
      luaL_argcheck(L, 0, 3, "expected numerical array");
   }
}
