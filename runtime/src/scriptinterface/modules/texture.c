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
#include <FlightlessManicotti/render/render.h>
#include "render/opengl/gl_render.h"

extern kl_render_context_t g_script_render_context;

const char* TEXTURE_LUA_LIB = "Texture";
GLenum tex_format_to_base_format(GLint tex_format);
GLenum tex_format_to_type(GLint tex_format);
int luaopen_texture(lua_State* L);

GLenum tex_format_to_base_format(GLint tex_format)
{
   GLenum ret = 0;
   switch(tex_format)
   {
      case GL_R8:
      case GL_R16:
      case GL_R16F:
      case GL_R32F:
         ret = GL_RED;
         break;

      case GL_RG8:
      case GL_RG16:
      case GL_RG16F:
      case GL_RG32F:
         ret = GL_RG;
         break;

      case GL_RGBA8:
      case GL_RGBA16:
      case GL_RGBA16F:
      case GL_RGBA32F:
         ret = GL_RGBA;
         break;
   }
   return ret;
}

GLenum tex_format_to_type(GLint tex_format)
{
   GLenum ret = 0;
   switch(tex_format)
   {
      case GL_R8:
      case GL_RG8:
      case GL_RGBA8:
         ret = GL_UNSIGNED_BYTE;
         break;

      case GL_R16:
      case GL_RG16:
      case GL_RGBA16:
         ret = GL_UNSIGNED_SHORT;
         break;

      case GL_R16F:
      case GL_R32F:
      case GL_RG16F:
      case GL_RG32F:
      case GL_RGBA16F:
      case GL_RGBA32F:
         ret = GL_FLOAT;
         break;
   }
   return ret;
}

static int Texture_new(lua_State* L)
{
   struct _kl_texture* texture = NULL;
   FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
   FIBITMAP *dib = NULL;
   uint8_t* bits = NULL;
   uint16_t width, height;
   const char* filename;
   GLenum internalFormat, srcFormat;

   if(lua_istable(L, 1))
   {
      texture = (struct _kl_texture*)lua_newuserdata(L, sizeof(struct _kl_texture));
      luaL_getmetatable(L, TEXTURE_LUA_LIB);
      lua_setmetatable(L, -2);

      texture->data_texture = 0;
      texture->tex_depth = GL_TEXTURE_2D;
      texture->tex_type = KL_TEXTURE_TYPE_BUFFER;

      /* Populate texture info */
      lua_pushstring(L, "width");
      lua_gettable(L, 1);
      texture->width = (uint16_t)lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_pushstring(L, "height");
      lua_gettable(L, 1);
      texture->height = (uint16_t)lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_pushstring(L, "format");
      lua_gettable(L, 1);
      texture->tex_format = (GLint)lua_tointeger(L, -1);
      lua_pop(L, 1);

      glGenTextures(1, &texture->texture);
      glBindTexture(GL_TEXTURE_2D, texture->texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, texture->tex_format, texture->width, texture->height,
                   0, tex_format_to_base_format(texture->tex_format),
                   tex_format_to_type(texture->tex_format), NULL);
      glBindTexture(GL_TEXTURE_2D, 0);

      return 1;
   }
   else if(lua_isnumber(L, 1))
   {
      texture = (struct _kl_texture*)lua_newuserdata(L, sizeof(struct _kl_texture));

      texture->data_texture = (uint16_t)lua_tointeger(L, 1);
      texture->tex_depth = 0;
      texture->texture = 0;
      texture->width = 0;
      texture->height = 0;
      texture->tex_format = 0;
      texture->tex_type = KL_TEXTURE_TYPE_DATA;

      luaL_getmetatable(L, TEXTURE_LUA_LIB);
      lua_setmetatable(L, -2);

      return 1;
   }

   /* Try to load the texture filename */
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
   width = (uint16_t)FreeImage_GetWidth(dib);
   height = (uint16_t)FreeImage_GetHeight(dib);

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

   texture->data_texture = 0;
   texture->tex_depth = GL_TEXTURE_2D; /* Hax */
   texture->width = width;
   texture->height = height;
   texture->tex_format = internalFormat;
   texture->tex_type = KL_TEXTURE_TYPE_IMAGE;

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      glGenTextures(1, &texture->texture);

      glBindTexture(texture->tex_depth, texture->texture);

      /* Slight hax, should be able to specify wrap mode */
      glTexParameteri(texture->tex_depth, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(texture->tex_depth, GL_TEXTURE_WRAP_T, GL_REPEAT);

      /* Slight hax, should be able to specify min/mag filter */
      glTexParameteri(texture->tex_depth, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(texture->tex_depth, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      switch(texture->tex_depth)
      {
         case GL_TEXTURE_2D:
         {
            glTexImage2D(texture->tex_depth, 0, internalFormat, width, height,
                         0, srcFormat, GL_UNSIGNED_BYTE, bits);
            break;
         }
      }

      /* Generate mip levels */
      glEnable(texture->tex_depth);
      glGenerateMipmap(texture->tex_depth);
      glDisable(texture->tex_depth);

      glBindTexture(texture->tex_depth, 0);
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

static int Texture_resize(lua_State* L)
{
   struct _kl_texture* texture = (struct _kl_texture*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->drawableCGLContext);
   CGLLockContext(g_script_render_context->drawableCGLContext);
   {
      glBindTexture(GL_TEXTURE_2D, texture->texture);
      glTexImage2D(GL_TEXTURE_2D, 0, texture->tex_format, texture->width, texture->height,
                   0, tex_format_to_base_format(texture->tex_format),
                   tex_format_to_type(texture->tex_format), NULL);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
   CGLUnlockContext(g_script_render_context->drawableCGLContext);
   return 0;
}

static const struct luaL_reg Texture_instance_methods [] = {
   {"resize", Texture_resize},
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

   lua_newtable(L);

   lua_pushnumber(L, GL_R8);
   lua_setfield(L, -2, "R8");

   lua_pushnumber(L, GL_R16);
   lua_setfield(L, -2, "R16");

   lua_pushnumber(L, GL_R16F);
   lua_setfield(L, -2, "R16F");

   lua_pushnumber(L, GL_R32F);
   lua_setfield(L, -2, "R32F");

   lua_pushnumber(L, GL_RG8);
   lua_setfield(L, -2, "RG8");

   lua_pushnumber(L, GL_RG16);
   lua_setfield(L, -2, "RG16");

   lua_pushnumber(L, GL_RG16F);
   lua_setfield(L, -2, "RG16F");

   lua_pushnumber(L, GL_RG32F);
   lua_setfield(L, -2, "RG32F");

   lua_pushnumber(L, GL_RGBA8);
   lua_setfield(L, -2, "RGBA8");

   lua_pushnumber(L, GL_RGBA16);
   lua_setfield(L, -2, "RGBA16");

   lua_pushnumber(L, GL_RGBA16F);
   lua_setfield(L, -2, "RGBA16F");

   lua_pushnumber(L, GL_RGBA32F);
   lua_setfield(L, -2, "RGBA32F");

   lua_setfield(L, -2, "format");

   return 1;
}
