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

#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <FlightlessManicotti/fm.h>

static int kl_cast_int_to_number(lua_State* L)
{
   lua_Integer iint = lua_tointeger(L, 1);
   lua_pushnumber(L, *((lua_Number*)&iint));
   return 1;
}

static const struct luaL_reg cast_module [] = {
    {"itof", kl_cast_int_to_number},
    {NULL, NULL}
};

int luaopen_cast(lua_State* L)
{
   luaL_register(L, "Cast", cast_module);
   return 1;
}