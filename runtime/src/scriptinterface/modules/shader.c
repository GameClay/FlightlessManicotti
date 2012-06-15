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

const char* SHADER_LUA_LIB = "Shader";
const char* SHADER_PROGRAM_LUA_LIB = "ShaderProgram";

extern kl_render_context_t g_script_render_context;

static int shader_compile(const char* shader_src, GLenum shader_type, GLuint* out_shader)
{
   int ret = KL_ERROR;

   if(shader_src != NULL)
   {
      int compile_success;
      GLuint gl_shader;

      CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
      CGLLockContext(g_script_render_context->resourceCGLContext);

      gl_shader = glCreateShader(shader_type);
      glShaderSource(gl_shader, 1, (const GLchar**)&shader_src, 0);
      glCompileShader(gl_shader);
      glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &compile_success);
      if(compile_success == GL_FALSE)
      {
         char* vertexInfoLog;
         int maxLength;
         glGetShaderiv(gl_shader, GL_INFO_LOG_LENGTH, &maxLength);
         vertexInfoLog = kl_heap_alloc(maxLength);

         glGetShaderInfoLog(gl_shader, maxLength, &maxLength, vertexInfoLog);
         kl_log_err(vertexInfoLog);
         kl_heap_free(vertexInfoLog);

         glDeleteShader(gl_shader);
      }
      else
      {
         *out_shader = gl_shader;
         ret = KL_SUCCESS;
      }

      CGLUnlockContext(g_script_render_context->resourceCGLContext);
   }

   return ret;
}

static int Shader_allocate(lua_State* L)
{
   struct _kl_shader* shader = NULL;
   GLuint gl_shader;
   GLenum shader_type;
   const char* shader_src = NULL;

   luaL_argcheck(L, lua_isstring(L, 1), 1, "expected shader source");
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected shader type");

   shader_src = lua_tostring(L, 1);
   shader_type = lua_tointeger(L, 2);

   if(shader_compile(shader_src, shader_type, &gl_shader) == KL_SUCCESS)
   {
      shader = (struct _kl_shader*)lua_newuserdata(L, sizeof(struct _kl_shader));
      shader->shader = gl_shader;
      shader->shader_type = shader_type;
      luaL_getmetatable(L, SHADER_LUA_LIB);
      lua_setmetatable(L, -2);
   }
   else
   {
      lua_pushstring(L, "error compiling shader");
      lua_error(L);
   }

   return 1;
}

static int Shader_gc(lua_State* L)
{
   struct _kl_shader* shader = (struct _kl_shader*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);

   glDeleteShader(shader->shader);

   CGLUnlockContext(g_script_render_context->resourceCGLContext);

   return 0;
}

static int Shader_update(lua_State* L)
{
   struct _kl_shader* shader = (struct _kl_shader*)lua_touserdata(L, 1);
   const char* shader_src = NULL;
   GLuint gl_shader;

   luaL_argcheck(L, lua_isstring(L, 2), 2, "expected shader source");

   shader_src = lua_tostring(L, 2);

   if(shader_compile(shader_src, shader->shader_type, &gl_shader) == KL_SUCCESS)
   {
      GLuint old_shader = shader->shader;
      shader->shader = gl_shader;
      glDeleteShader(old_shader);

      lua_pushboolean(L, 1);
   }
   else
   {
      lua_pushboolean(L, 0);
   }

   return 1;
}

static const struct luaL_reg Shader_instance_methods [] = {
   {"update", Shader_update},
   {NULL, NULL}
};

static const struct luaL_reg Shader_class_methods [] = {
   {"allocate", Shader_allocate},
   {NULL, NULL}
};

/* ShaderProgram */
static int program_link(struct _kl_shader** shaders, int num_shaders, GLuint* out_program)
{
   int ret = KL_ERROR;
   int i;
   GLint link_success;
   GLuint program;

   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);

   program = glCreateProgram();

   for(i = 0; i < num_shaders; i++)
   {
      if(shaders[i] != NULL) glAttachShader(program, shaders[i]->shader);
   }

   /* Bind mesh attribute locations */
   glBindAttribLocation(program, KL_MESH_POSITION_IDX, "InPosition");
   glBindAttribLocation(program, KL_MESH_NORMAL_IDX, "InNormal");
   glBindAttribLocation(program, KL_MESH_TEX0_IDX, "InTex0");
   glBindAttribLocation(program, KL_MESH_COLOR_IDX, "InColor");

   glLinkProgram(program);

   /* Check link status */
   glGetProgramiv(program, GL_LINK_STATUS, &link_success);
   if(link_success == GL_TRUE)
   {
      *out_program = program;
      ret = KL_SUCCESS;
   }
   else
   {
      char* linkInfoLog;
      int maxLength;

      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
      linkInfoLog = kl_heap_alloc(maxLength);

      glGetProgramInfoLog(program, maxLength, &maxLength, linkInfoLog);
      kl_log_err(linkInfoLog);
      kl_heap_free(linkInfoLog);

      glDeleteProgram(program);
   }

   CGLUnlockContext(g_script_render_context->resourceCGLContext);

   return ret;
}

static int ShaderProgram_allocate(lua_State* L)
{
   struct _kl_effect* effect = NULL;
   GLuint gl_program;

   /* Hacky */
   struct _kl_shader* shaders[3] = {NULL, NULL, NULL};
   int num_shaders = 3;

   /* TODO: Arg type checking */
   shaders[0] = lua_touserdata(L, 1);
   shaders[1] = lua_touserdata(L, 2);
   shaders[2] = lua_touserdata(L, 3);

   if(program_link(shaders, num_shaders, &gl_program) == KL_SUCCESS)
   {
      effect = (struct _kl_effect*)lua_newuserdata(L, sizeof(struct _kl_effect));
      effect->program = gl_program;
      luaL_getmetatable(L, SHADER_PROGRAM_LUA_LIB);
      lua_setmetatable(L, -2);
   }
   else
   {
      lua_pushstring(L, "error linking shader program");
      lua_error(L);
   }

   return 1;
}

static int ShaderProgram_update(lua_State* L)
{
   struct _kl_effect* effect = (struct _kl_effect*)lua_touserdata(L, 1);
   GLuint gl_program;

   /* Hacky */
   struct _kl_shader* shaders[3] = {NULL, NULL, NULL};
   int num_shaders = 3;

   /* TODO: Arg type checking */
   shaders[0] = lua_touserdata(L, 2);
   shaders[1] = lua_touserdata(L, 3);
   shaders[2] = lua_touserdata(L, 4);

   if(program_link(shaders, num_shaders, &gl_program) == KL_SUCCESS)
   {
      GLuint old_program = effect->program;
      effect->program = gl_program;
      glDeleteProgram(old_program);
   }
   else
   {
      lua_pushstring(L, "error linking shader program");
      lua_error(L);
   }

   return 1;
}

static int ShaderProgram_gc(lua_State* L)
{
   struct _kl_effect* effect = (struct _kl_effect*)lua_touserdata(L, 1);

   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);

   glDeleteProgram(effect->program);

   CGLUnlockContext(g_script_render_context->resourceCGLContext);

   return 0;
}

static const struct luaL_reg ShaderProgram_instance_methods [] = {
   {"update", ShaderProgram_update},
   {NULL, NULL}
};

static const struct luaL_reg ShaderProgram_class_methods [] = {
   {"allocate", ShaderProgram_allocate},
   {NULL, NULL}
};

int luaopen_shader(lua_State* L)
{
   /* Shader */
   luaL_newmetatable(L, SHADER_LUA_LIB);
   luaL_register(L, 0, Shader_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, Shader_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, SHADER_LUA_LIB, Shader_class_methods);

   /* Shader type constant */
   lua_newtable(L);

   lua_pushnumber(L, GL_VERTEX_SHADER);
   lua_setfield(L, -2, "vertex");

   lua_pushnumber(L, GL_FRAGMENT_SHADER);
   lua_setfield(L, -2, "fragment");

   lua_pushnumber(L, GL_GEOMETRY_SHADER);
   lua_setfield(L, -2, "geometry");

   lua_setfield(L, -2, "type");

   /* ShaderProgram */
   luaL_newmetatable(L, SHADER_PROGRAM_LUA_LIB);
   luaL_register(L, 0, ShaderProgram_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, ShaderProgram_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, SHADER_PROGRAM_LUA_LIB, ShaderProgram_class_methods);

   return 1;
}
