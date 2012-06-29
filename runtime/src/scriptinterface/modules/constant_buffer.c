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
#include <FlightlessManicotti/render/shader/constant_buffer.h>
#include "render/opengl/gl_render.h"

const char* SHADER_CONSTANT_LUA_LIB = "ShaderConstant";
const char* CONSTANT_BUFFER_LUA_LIB = "ConstantBuffer";
int luaopen_shader_constant(lua_State* L);
void GLenumToKLShaderConstant(GLenum type, kl_new_shader_constant_t* constant);

extern kl_render_context_t g_script_render_context;
extern const char* SHADER_PROGRAM_LUA_LIB;

void GLenumToKLShaderConstant(GLenum type, kl_new_shader_constant_t* constant)
{
   switch(type)
   {
      case GL_FLOAT:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
         constant->constant_sz = 1;
         break;
      case GL_FLOAT_VEC2:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
         constant->constant_sz = 2;
         break;
      case GL_FLOAT_VEC3:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
         constant->constant_sz = 3;
         break;
      case GL_FLOAT_VEC4:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
         constant->constant_sz = 4;
         break;

      case GL_INT:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_INT;
         constant->constant_sz = 1;
         break;
      case GL_INT_VEC2:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_INT;
         constant->constant_sz = 2;
         break;
      case GL_INT_VEC3:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_INT;
         constant->constant_sz = 3;
         break;
      case GL_INT_VEC4:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_INT;
         constant->constant_sz = 4;
         break;

      case GL_FLOAT_MAT2:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_MATRIX;
         constant->constant_sz = 2;
         break;
      case GL_FLOAT_MAT3:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_MATRIX;
         constant->constant_sz = 3;
         break;
      case GL_FLOAT_MAT4:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_MATRIX;
         constant->constant_sz = 4;
         break;

      case GL_SAMPLER_1D:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
         constant->constant_sz = 1;
         break;
      case GL_SAMPLER_2D:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
         constant->constant_sz = 2;
         break;
      case GL_SAMPLER_3D:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
         constant->constant_sz = 3;
         break;
      case GL_SAMPLER_CUBE:
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
         constant->constant_sz = 4;
         break;
   }
}

static int ConstantBuffer_new(lua_State* L)
{
   struct _kl_effect* effect = (struct _kl_effect*)luaL_checkudata(L, 1, SHADER_PROGRAM_LUA_LIB);
   GLint count, i;
   GLint uniform_num;
   GLenum uniform_type;
   GLchar name[512];
   GLsizei name_sz = sizeof(name);
   kl_shader_constant_buffer_t* constant_buffer;

   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);

   glGetProgramiv(effect->program, GL_ACTIVE_UNIFORMS, &count);

   lua_newtable(L);
   constant_buffer = lua_newuserdata(L, sizeof(kl_shader_constant_buffer_t) +
      count * sizeof(kl_new_shader_constant_t));
   luaL_getmetatable(L, CONSTANT_BUFFER_LUA_LIB);
   lua_setmetatable(L, -2);
   lua_setfield(L, -2, "_constantbuffer");

   for(i = 0; i < count; i++)
   {
      kl_new_shader_constant_t** constant;

      glGetActiveUniform(effect->program, i, name_sz, NULL, &uniform_num,
         &uniform_type, name);
      GLenumToKLShaderConstant(uniform_type, &constant_buffer->constant[i]);
      constant_buffer->constant[i].constant_num = uniform_num;
      constant_buffer->constant[i].constant_idx = i;

      constant = lua_newuserdata(L, sizeof(kl_new_shader_constant_t*));
      *constant = &constant_buffer->constant[i];
      luaL_getmetatable(L, SHADER_CONSTANT_LUA_LIB);
      lua_setmetatable(L, -2);
      lua_setfield(L, -2, name);
   }

   CGLUnlockContext(g_script_render_context->resourceCGLContext);

   return 1;
}

static int ConstantBuffer_gc(lua_State* L)
{
   KL_UNUSED(L);
   kl_log("ConstantBuffer - GC called!");
   return 0;
}

static const struct luaL_reg ConstantBuffer_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg ConstantBuffer_class_methods [] = {
   {"new", ConstantBuffer_new},
   {NULL, NULL}
};

/***/

static int ShaderConstant_gc(lua_State* L)
{
   KL_UNUSED(L);
   return 0;
}

static int ShaderConstant_tostring(lua_State* L)
{
   kl_new_shader_constant_t* constant = *((kl_new_shader_constant_t**)lua_touserdata(L, 1));
   const char* typename = "";
   switch(constant->constant_type)
   {
      case KL_SHADER_CONSTANT_TYPE_MATRIX: typename = "matrix"; break;
      case KL_SHADER_CONSTANT_TYPE_FLOAT: typename = "float"; break;
      case KL_SHADER_CONSTANT_TYPE_INT: typename = "int"; break;
      case KL_SHADER_CONSTANT_TYPE_TEX: typename = "sampler"; break;
   }

   lua_pushfstring(L, "{index: %d, type: %s%d[%d]}", constant->constant_idx, typename,
      constant->constant_sz, constant->constant_num);
   return 1;
}

static const struct luaL_reg ShaderConstant_instance_methods [] = {
   {NULL, NULL}
};

static const struct luaL_reg ShaderConstant_class_methods [] = {
   {NULL, NULL}
};

int luaopen_shader_constant(lua_State* L)
{
   /* Constant buffer */
   luaL_newmetatable(L, CONSTANT_BUFFER_LUA_LIB);
   luaL_register(L, 0, ConstantBuffer_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, ConstantBuffer_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, CONSTANT_BUFFER_LUA_LIB, ConstantBuffer_class_methods);

   /* Shader constant */
   luaL_newmetatable(L, SHADER_CONSTANT_LUA_LIB);
   luaL_register(L, 0, ShaderConstant_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, ShaderConstant_gc);
   lua_setfield(L, -2, "__gc");
   lua_pushcfunction(L, ShaderConstant_tostring);
   lua_setfield(L, -2, "__tostring");

   luaL_register(L, SHADER_CONSTANT_LUA_LIB, ShaderConstant_class_methods);

   return 1;
}
