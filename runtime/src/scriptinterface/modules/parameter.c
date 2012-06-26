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
#include <FlightlessManicotti/game/parameters/parameter.h>

#define PARAMETERVALUE_LIB "ParameterValue"

static int ParameterValue_new(lua_State* L)
{
   kl_parameter_t param = NULL;

   luaL_argcheck(L, lua_isstring(L, 1), 1, "expected parameter name");

   param = (kl_parameter_t)lua_newuserdata(L, sizeof(_kl_parameter));
   param->value = 42.0f;

   /* Register parameter */

   luaL_getmetatable(L, PARAMETERVALUE_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int ParameterValue_gc(lua_State* L)
{
   kl_parameter_t param = (kl_parameter_t)lua_touserdata(L, 1);
   if(param != NULL)
   {
      /* Unregister */
   }
   return 0;
}

static int ParameterValue_getvalue(lua_State* L)
{
   kl_parameter_t param = (kl_parameter_t)lua_touserdata(L, 1);
   lua_pushnumber(L, param->value);
   return 1;
}

static int ParameterValue_setvalue(lua_State* L)
{
   kl_parameter_t param = (kl_parameter_t)lua_touserdata(L, 1);
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected number");
   param->value = (float)lua_tonumber(L, 2);
   return 0;
}

static const struct luaL_reg ParameterValue_instance_methods [] = {
   {"setvalue", ParameterValue_setvalue},
   {"getvalue", ParameterValue_getvalue},
   {NULL, NULL}
};

static const struct luaL_reg ParameterValue_class_methods [] = {
   {"new", ParameterValue_new},
   {NULL, NULL}
};

int luaopen_parameter(lua_State* L)
{
   luaL_newmetatable(L, PARAMETERVALUE_LIB);
   luaL_register(L, 0, ParameterValue_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, ParameterValue_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, PARAMETERVALUE_LIB, ParameterValue_class_methods);

   return 1;
}
