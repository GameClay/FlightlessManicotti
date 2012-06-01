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
#include <sanskrit/sklog.h>
#include <FlightlessManicotti/render/render.h>
#include "render/opengl/gl_render.h"

extern kl_render_context_t g_script_render_context;

const char* RENDER_TARGET_LUA_LIB = "RenderTarget";

static int RenderTarget_new(lua_State* L)
{
   struct _kl_offscreen_target* target = NULL;

   luaL_argcheck(L, lua_isnumber(L, 1), 1, "expected target width");
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected target height");

   target = (struct _kl_offscreen_target*)lua_newuserdata(L, sizeof(struct _kl_offscreen_target));
   target->width = lua_tointeger(L, 1);
   target->height = lua_tointeger(L, 2);

   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);
   {
      glGenFramebuffersEXT(1, &target->framebuffer);
      glGenTextures(1, &target->texture);
      glGenRenderbuffersEXT(1, &target->depthstencil);

      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, target->framebuffer);

      glBindTexture(GL_TEXTURE_2D, target->texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, target->width, target->height,
                   0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                GL_TEXTURE_2D, target->texture, 0);

      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, target->depthstencil);
      glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, target->width, target->height);
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
                                   target->depthstencil);

      if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
      {
         glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
         CGLUnlockContext(g_script_render_context->resourceCGLContext);
         skerr("Script render target creation failed.");
         lua_pop(L, 1);
         lua_pushnil(L);
         return 1;
      }

      /* Clear framebuffer */
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   }
   CGLUnlockContext(g_script_render_context->resourceCGLContext);

   luaL_getmetatable(L, RENDER_TARGET_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int RenderTarget_gc(lua_State* L)
{
   struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);
   {
      glDeleteFramebuffersEXT(1, &target->framebuffer);
      glDeleteRenderbuffersEXT(1, &target->depthstencil);
      glDeleteTextures(1, &target->texture);
   }
   CGLUnlockContext(g_script_render_context->resourceCGLContext);
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
