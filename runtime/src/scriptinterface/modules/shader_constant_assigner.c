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
#include "render/opengl/gl_render.h"

const char* SHADER_CONSTANT_ASSIGNER_LUA_LIB = "ShaderConstantAssigner";

#include <FlightlessManicotti/core/timer.h>
extern kl_absolute_time_t g_mainloop_elapsed_time;
static void _elapsed_time_ms_assigner(const void* context, int32_t location, kl_shader_constant_ptr constant)
{
   uint64_t elapsed_time_ns;
   kl_absolute_time_to_ns(&g_mainloop_elapsed_time, &elapsed_time_ns);
   glUniform1f(location, (float)elapsed_time_ns * 1e-6);
}

int luaopen_shader_constant_assigner(lua_State* L)
{
   kl_shader_constant_assigner_t* assigner = NULL;

   /* Shader Constant Assigner Function */
   luaL_newmetatable(L, SHADER_CONSTANT_ASSIGNER_LUA_LIB);

   /* Single float assigners */
   lua_newtable(L);

   /* Elapsed Time in milliseconds */
   assigner = (kl_shader_constant_assigner_t*)lua_newuserdata(L, sizeof(kl_shader_constant_assigner_t));
   assigner->context = NULL;
   assigner->assigner = _elapsed_time_ms_assigner;
   lua_setfield(L, -2, "elapsed_time_ms");

   lua_setfield(L, -2, "float");

   lua_pop(L, 1);

   return 1;
}
