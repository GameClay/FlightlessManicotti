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
#include "fm.h"

extern const char* VECTOR2D_LUA_LIB;

void lua_readvector2d(lua_State* L, int param_idx, float* out_xy)
{
   float* tempxy;
   int top;
   
   if(lua_istable(L, param_idx))
   {
      top = lua_gettop(L);
      lua_pushinteger(L, 1);
      lua_gettable(L, param_idx);
      lua_pushinteger(L, 2);
      lua_gettable(L, param_idx);
      luaL_argcheck(L, lua_isnumber(L, top + 1) && lua_isnumber(L, top + 2), 
         param_idx, "expected numerical array");
      out_xy[0] = lua_tonumber(L, top + 1);
      out_xy[1] = lua_tonumber(L, top + 2);
   }
   else if(lua_isuserdata(L, 3))
   {
      // Won't return if error
      tempxy = (float*)luaL_checkudata(L, param_idx, VECTOR2D_LUA_LIB);
      out_xy[0] = tempxy[0];
      out_xy[1] = tempxy[1];
   }
   else
   {
      luaL_argcheck(L, 0, 3, "expected numerical array, or vector2d");
   }
}