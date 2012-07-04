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
void GLenumToKLShaderConstant(GLenum type, kl_shader_constant_t* constant);

extern kl_render_context_t g_script_render_context;
extern const char* SHADER_PROGRAM_LUA_LIB;
extern const char* TEXTURE_LUA_LIB;
extern const char* RENDER_TARGET_LUA_LIB;

void GLenumToKLShaderConstant(GLenum type, kl_shader_constant_t* constant)
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
      count * sizeof(kl_shader_constant_t));
   kl_zero_mem(constant_buffer, sizeof(kl_shader_constant_buffer_t) +
      count * sizeof(kl_shader_constant_t));
   constant_buffer->num_constants = count;
   luaL_getmetatable(L, CONSTANT_BUFFER_LUA_LIB);
   lua_setmetatable(L, -2);
   lua_setfield(L, -2, "_constantbuffer");

   for(i = 0; i < count; i++)
   {
      kl_shader_constant_t** constant;
      char* arrayLoc = NULL;

      glGetActiveUniform(effect->program, i, name_sz, NULL, &uniform_num,
         &uniform_type, name);

      arrayLoc = strstr(name, "[0]");
      if(arrayLoc != NULL) *arrayLoc = '\0';

      GLenumToKLShaderConstant(uniform_type, &constant_buffer->constant[i]);
      constant_buffer->constant[i].constant_num = uniform_num;
      constant_buffer->constant[i].constant_loc =
         glGetUniformLocation(effect->program, name);

      /* Look for matrices */
      if(strcmp(name, "object_to_screen") == 0)
      {
         constant_buffer->constant[i].data_constant = kl_matrix_data_object_to_screen;
      }

      constant = lua_newuserdata(L, sizeof(kl_shader_constant_t*));
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
   kl_shader_constant_buffer_t* constant_buffer = (kl_shader_constant_buffer_t*)lua_touserdata(L, 1);
   size_t i;
   for(i = 0; i < constant_buffer->num_constants; i++)
   {
      if(constant_buffer->constant[i].dealloc_constant)
      {
         kl_heap_free(constant_buffer->constant[i].constant.as_ptr);
      }
   }

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
   kl_shader_constant_t* constant = *((kl_shader_constant_t**)lua_touserdata(L, 1));
   const char* typename = "";
   switch(constant->constant_type)
   {
      case KL_SHADER_CONSTANT_TYPE_MATRIX: typename = "matrix"; break;
      case KL_SHADER_CONSTANT_TYPE_FLOAT: typename = "float"; break;
      case KL_SHADER_CONSTANT_TYPE_INT: typename = "int"; break;
      case KL_SHADER_CONSTANT_TYPE_TEX: typename = "sampler"; break;
   }

   lua_pushfstring(L, "{location: %d, type: %s%d[%d]}", constant->constant_loc, typename,
      constant->constant_sz, constant->constant_num);
   return 1;
}


static int ShaderConstant_set(lua_State* L)
{
   kl_shader_constant_t* constant = *((kl_shader_constant_t**)lua_touserdata(L, 1));
   void* old_ptr = NULL;
   int l_index = 2;

   switch(lua_type(L, l_index))
   {
      case LUA_TNUMBER:
      {
         if(constant->constant_type != KL_SHADER_CONSTANT_TYPE_FLOAT ||
            constant->constant.as_float_ptr == NULL)
         {
            if(constant->dealloc_constant)
            {
               old_ptr = constant->constant.as_ptr;
            }

            constant->constant.as_float_ptr = kl_heap_alloc(sizeof(float));
         }

         constant->dealloc_constant = 1;
         constant->constant_sz = 1;
         constant->constant_num = 1;
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
         *constant->constant.as_float_ptr = (float)lua_tonumber(L, l_index);
         break;
      }

      case LUA_TTABLE:
      {
         size_t tsize = 0;
         int csize = 0;

         /* Get size of each element */
         lua_pushnil(L);
         while(lua_next(L, l_index))
         {
            switch(lua_type(L, -1))
            {
               case LUA_TNUMBER:
               {
                  if(csize == 0) csize = 1;
                  else
                  {
                     if(csize != 1)
                     {
                        kl_log_err("All elements of a shader constant table assignment must be the same size.");
                     }
                  }
                  break;
               }

               case LUA_TTABLE:
               {
                  if(csize == 0) csize = (int)lua_objlen(L, -1);
                  else
                  {
                     if(csize != (int)lua_objlen(L, -1))
                     {
                        kl_log_err("All elements of a shader constant table assignment must be the same size.");
                     }
                  }
                  break;
               }
            }
            tsize++;
            lua_pop(L, 1);
         }
         if(tsize == 0) lua_pop(L, 1);

         if(constant->constant_type != KL_SHADER_CONSTANT_TYPE_FLOAT ||
            (constant->constant_num > 0 && constant->constant_num != tsize) ||
            (constant->constant_sz > 0 && constant->constant_sz != csize) ||
            constant->constant.as_float_ptr == NULL)
         {
            if(constant->dealloc_constant)
            {
               old_ptr = constant->constant.as_ptr;
            }

            constant->constant.as_float_ptr = kl_heap_alloc(tsize * csize * sizeof(float));
         }

         constant->constant_num = (uint16_t)tsize;
         constant->constant_sz = (uint16_t)csize;
         constant->dealloc_constant = 1;
         constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;

         if(tsize > 0)
         {
            float* cur_f = constant->constant.as_float_ptr;
            lua_pushnil(L);
            while(lua_next(L, l_index))
            {
               switch(lua_type(L, -1))
               {
                  case LUA_TNUMBER:
                  {
                     *cur_f = (float)lua_tonumber(L, -1);
                     cur_f++;
                     break;
                  }

                  case LUA_TTABLE:
                  {
                     lua_pushnil(L);
                     while(lua_next(L, -2))
                     {
                        *cur_f = (float)lua_tonumber(L, -1);
                        cur_f++;
                        lua_pop(L, 1);
                     }
                     break;
                  }
               }
               lua_pop(L, 1);
            }
         }
         break;
      }

      case LUA_TUSERDATA:
      {
         int is_eq = 0;

         /* Check to see if it's a render target */
         lua_getmetatable(L, l_index);
         luaL_getmetatable(L, RENDER_TARGET_LUA_LIB);
         is_eq = lua_equal(L, -1, -2);
         lua_pop(L, 2);
         if(is_eq)
         {
            struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, l_index);
            constant->dealloc_constant = 0;
            constant->constant_sz = 2; /* 2D texture */
            constant->constant_num = 1;
            constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
            constant->constant.as_tex = target->texture[0]->texture; /* HAX */
            break;
         }

         /* Check to see if it's a texture */
         lua_getmetatable(L, l_index);
         luaL_getmetatable(L, TEXTURE_LUA_LIB);
         is_eq = lua_equal(L, -1, -2);
         lua_pop(L, 2);
         if(is_eq)
         {
            struct _kl_texture* tex = (struct _kl_texture*)lua_touserdata(L, l_index);
            constant->dealloc_constant = 0;
            constant->constant_num = 1;

            if(tex->data_texture)
            {
               constant->constant_type = KL_SHADER_CONSTANT_TYPE_DATA;
               constant->constant.as_tex = tex->data_texture;
               constant->constant_sz = 0;
            }
            else
            {
               switch(tex->tex_depth)
               {
                  case GL_TEXTURE_2D:
                  {
                     constant->constant_sz = 2; /* 2D texture */
                     break;
                  }
               }

               constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
               constant->constant.as_tex = tex->texture;
            }
            break;
         }
         break;
      }

      default:
      {
         break;
      }
   }

   if(old_ptr != NULL)
   {
      kl_heap_free(old_ptr);
   }

   return 1;
}

static const struct luaL_reg ShaderConstant_instance_methods [] = {
   {"set", ShaderConstant_set},
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
