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
#include <FlightlessManicotti/render/render.h>
#include "render/opengl/gl_render.h"

extern kl_render_context_t g_script_render_context;

const char* RENDER_TARGET_LUA_LIB = "RenderTarget";

static int RenderTarget_new(lua_State* L)
{
   struct _kl_offscreen_target* inst =
      (struct _kl_offscreen_target*)lua_newuserdata(L, sizeof(struct _kl_offscreen_target));

   KL_UNUSED(inst);

   luaL_getmetatable(L, RENDER_TARGET_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int RenderTarget_gc(lua_State* L)
{
   return 0;
}


static const struct luaL_reg RenderTarget_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg RenderTarget_class_methods [] = {
   {"new", RenderTarget_new},
   {NULL, NULL}
};

int luaopen_render_target(lua_State* L)
{
   luaL_newmetatable(L, RENDER_TARGET_LUA_LIB);
   luaL_register(L, 0, RenderTarget_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, RenderTarget_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, RENDER_TARGET_LUA_LIB, RenderTarget_class_methods);

   return 1;
}
