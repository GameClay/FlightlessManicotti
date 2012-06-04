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
#include <FreeImage.h>
#include <sanskrit/sklog.h>
#include <FlightlessManicotti/render/render.h>
#include "render/opengl/gl_render.h"

extern kl_render_context_t g_script_render_context;

const char* TEXTURE_LUA_LIB = "Texture";

static int Texture_new(lua_State* L)
{
   struct _kl_texture* texture = NULL;
   FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
   FIBITMAP *dib = NULL;
   uint8_t* bits = NULL;
   uint32_t width, height;
   const char* filename;
   GLenum internalFormat, srcFormat;

   luaL_argcheck(L, lua_isstring(L, 1), 1, "expected texture filename");
   filename = lua_tostring(L, 1);

   /* Determine file type */
   fif = FreeImage_GetFileType(filename, 0);
   if(fif == FIF_UNKNOWN)
      fif = FreeImage_GetFIFFromFilename(filename);
   if(fif == FIF_UNKNOWN)
   {
      lua_pushnil(L);
      return 1;
   }

   /* Load image */
   if(FreeImage_FIFSupportsReading(fif))
      dib = FreeImage_Load(fif, filename, 0);
   else
   {
      lua_pushnil(L);
      return 1;
   }

   /* Get data */
   bits = FreeImage_GetBits(dib);
   width = FreeImage_GetWidth(dib);
   height = FreeImage_GetHeight(dib);

   /* Sanity check data */
   if((bits == 0) || (width == 0) || (height == 0))
   {
      FreeImage_Unload(dib);
      lua_pushnil(L);
      return 1;
   }

   /* Determine additional texture info */
   switch(FreeImage_GetColorType(dib))
   {
      case FIC_RGB:
      {
         internalFormat = GL_RGB8;
         srcFormat = GL_RGB;
         break;
      }

      case FIC_RGBALPHA:
      {
         internalFormat = GL_RGBA8;
         srcFormat = GL_RGBA;
         break;
      }

      default:
      {
         FreeImage_Unload(dib);
         lua_pushnil(L);
         return 1;
      }
   }

   texture = (struct _kl_texture*)lua_newuserdata(L, sizeof(struct _kl_texture));

   texture->tex_type = GL_TEXTURE_2D; /* Hax */

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      glGenTextures(1, &texture->texture);

      glBindTexture(texture->tex_type, texture->texture);

      /* Slight hax, should be able to specify min/mag filter */
      glTexParameteri(texture->tex_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(texture->tex_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      switch(texture->tex_type)
      {
         case GL_TEXTURE_2D:
         {
            glTexImage2D(texture->tex_type, 0 /* TODO: Mip-levels */, internalFormat, width, height,
                         0, srcFormat, GL_UNSIGNED_BYTE, bits);
            break;
         }
      }

      glBindTexture(texture->tex_type, 0);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);

   FreeImage_Unload(dib);

   luaL_getmetatable(L, TEXTURE_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int Texture_gc(lua_State* L)
{
   struct _kl_texture* texture = (struct _kl_texture*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      glDeleteTextures(1, &texture->texture);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);
   return 0;
}

static const struct luaL_reg Texture_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg Texture_class_methods [] = {
   {"new", Texture_new},
   {NULL, NULL}
};

int luaopen_texture(lua_State* L)
{
   luaL_newmetatable(L, TEXTURE_LUA_LIB);
   luaL_register(L, 0, Texture_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, Texture_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, TEXTURE_LUA_LIB, Texture_class_methods);

   return 1;
}
