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
#include <FlightlessManicotti/render/opengl/gl_render.h>

extern kl_render_context_t g_script_render_context;

const char* RENDER_TARGET_LUA_LIB = "RenderTarget";
int luaopen_render_target(lua_State* L);

extern const char* TEXTURE_LUA_LIB;

static int RenderTarget_new(lua_State* L)
{
   struct _kl_offscreen_target* target = NULL;

   luaL_argcheck(L, lua_isnumber(L, 1), 1, "expected target width");
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected target height");

   target = (struct _kl_offscreen_target*)lua_newuserdata(L, sizeof(struct _kl_offscreen_target));
   target->width = (GLuint)lua_tointeger(L, 1);
   target->height = (GLuint)lua_tointeger(L, 2);
   kl_zero_mem(target->texture, sizeof(target->texture));

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      glGenFramebuffers(1, &target->framebuffer);

      glGenRenderbuffers(1, &target->depthstencil);

      glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);

      glBindRenderbuffer(GL_RENDERBUFFER, target->depthstencil);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, target->width, target->height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                target->depthstencil);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);

   luaL_getmetatable(L, RENDER_TARGET_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int RenderTarget_gc(lua_State* L)
{
   struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      glDeleteFramebuffers(1, &target->framebuffer);
      glDeleteRenderbuffers(1, &target->depthstencil);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);
   return 0;
}

extern GLenum tex_format_to_base_format(GLint tex_format);
extern GLenum tex_format_to_type(GLint tex_format);

static int RenderTarget_update(lua_State* L)
{
   struct _kl_offscreen_target* target = NULL;

   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected target width");
   luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected target height");

   target = (struct _kl_offscreen_target*)lua_touserdata(L, 1);
   target->width = (GLuint)lua_tointeger(L, 2);
   target->height = (GLuint)lua_tointeger(L, 3);

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      int i;
      glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);

      glBindRenderbuffer(GL_RENDERBUFFER, target->depthstencil);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, target->width, target->height);

      for(i = 0; i < KL_OFFSCREEN_TARGET_MAX_TEX; i++)
      {
         struct _kl_texture* texture = target->texture[i];
         if(texture != NULL)
         {
            texture->width = (uint16_t)target->width;
            texture->height = (uint16_t)target->height;
            glBindTexture(GL_TEXTURE_2D, texture->texture);
            glTexImage2D(GL_TEXTURE_2D, 0, texture->tex_format, texture->width, texture->height,
                         0, tex_format_to_base_format(texture->tex_format),
                         tex_format_to_type(texture->tex_format), NULL);
         }
      }
      glBindTexture(GL_TEXTURE_2D, 0);

      /* Clear framebuffer */
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);

   return 0;
}

static int RenderTarget_bindtexture(lua_State* L)
{
   struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, 1);
   struct _kl_texture* texture = (struct _kl_texture*)luaL_checkudata(L, 2, TEXTURE_LUA_LIB);
   int attachment_idx = 0;

   if(!lua_isnoneornil(L, 3))
   {
      attachment_idx = (int)lua_tointeger(L, 3);
      attachment_idx = (attachment_idx < KL_OFFSCREEN_TARGET_MAX_TEX ? attachment_idx : 0);
   }

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      GLenum status;
      target->texture[attachment_idx] = texture;
      glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);

      glBindTexture(GL_TEXTURE_2D, texture->texture);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment_idx,
                             GL_TEXTURE_2D, texture->texture, 0);

      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE)
      {
         glBindTexture(GL_TEXTURE_2D, 0);
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
         CGLUnlockContext(g_script_render_context->drawableCGLContext);
         kl_log_err("Script render target bind texture failed.");
         return 0;
      }

      /* Clear framebuffer */
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);

   return 0;
}

static int RenderTarget_clear(lua_State* L)
{
   struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);
   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   CGLUnlockContext(g_script_render_context->drawableCGLContext);
   return 0;
}

static const struct luaL_reg RenderTarget_instance_methods [] = {
   {"bindtexture", RenderTarget_bindtexture},
   {"update", RenderTarget_update},
   {"clear", RenderTarget_clear},
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

   /* Buffer attachments */
   lua_newtable(L);

   lua_pushnumber(L, GL_COLOR_ATTACHMENT0);
   lua_setfield(L, -2, "color0");
   lua_pushnumber(L, GL_COLOR_ATTACHMENT1);
   lua_setfield(L, -2, "color1");
   lua_pushnumber(L, GL_COLOR_ATTACHMENT2);
   lua_setfield(L, -2, "color2");
   lua_pushnumber(L, GL_COLOR_ATTACHMENT3);
   lua_setfield(L, -2, "color3");
   lua_pushnumber(L, GL_DEPTH_ATTACHMENT);
   lua_setfield(L, -2, "depth");

   lua_setfield(L, -2, "attachment");

   return 1;
}
