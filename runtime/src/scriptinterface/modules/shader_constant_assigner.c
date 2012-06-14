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
#include <FlightlessManicotti/render/shader/shader_constant.h>

const char* SHADER_CONSTANT_ASSIGNER_LUA_LIB = "ShaderConstantAssigner";

#include <FlightlessManicotti/core/timer.h>
extern kl_absolute_time_t g_mainloop_elapsed_time;
void _elapsed_time_ms_assigner(const void* context, kl_shader_constant_ptr constant)
{
   static float tempf;
   uint64_t elapsed_time_ns;
   kl_absolute_time_to_ns(&g_mainloop_elapsed_time, &elapsed_time_ns);
   tempf = (float)elapsed_time_ns * 1e-6;

   constant->constant.as_float_ptr = &tempf;
   constant->constant_sz = 1;
   constant->constant_num = 1;
   constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
   constant->dealloc_constant = 0;
}

static const struct luaL_reg ShaderConstantAssigner_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg ShaderConstantAssigner_class_methods [] = {
   {NULL, NULL}
};

int luaopen_shader_constant_assigner(lua_State* L)
{
   kl_shader_constant_assigner_t* assigner = NULL;

   luaL_newmetatable(L, SHADER_CONSTANT_ASSIGNER_LUA_LIB);
   luaL_register(L, 0, ShaderConstantAssigner_instance_methods);
   lua_pushvalue(L, -1);

   luaL_register(L, SHADER_CONSTANT_ASSIGNER_LUA_LIB,
      ShaderConstantAssigner_class_methods);

   /* Single float assigners */
   lua_newtable(L);

   /* Elapsed Time in milliseconds */
   assigner = (kl_shader_constant_assigner_t*)lua_newuserdata(L, sizeof(kl_shader_constant_assigner_t));
   luaL_getmetatable(L, SHADER_CONSTANT_ASSIGNER_LUA_LIB);
   lua_setmetatable(L, -2);

   assigner->context = NULL;
   assigner->assigner = _elapsed_time_ms_assigner;

   lua_setfield(L, -2, "elapsed_time_ms");

   lua_setfield(L, -2, "float");

   return 1;
}
