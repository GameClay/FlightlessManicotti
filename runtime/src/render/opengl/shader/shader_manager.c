/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2011 GameClay LLC
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

#include <FlightlessManicotti/render/shader/shader_manager.h>
#include <sanskrit/sklog.h>
#include "render/script/getshader.h"
#include <stdio.h>

/* hax */
#include "render/opengl/gl_render.h"
#include <OpenGL/gl.h>

int kl_shader_manager_create(kl_shader_manager_t* manager, uint32_t num_shaders, kl_render_context_t render_ctx)
{
   int ret = KL_ERROR;

   if(manager != NULL)
   {
      kl_shader_manager_t mgr = kl_heap_alloc(sizeof(_kl_shader_manager));
      if(mgr != NULL)
      {
         mgr->shader = kl_heap_alloc(sizeof(struct _kl_shader) * num_shaders);
         mgr->num_shaders = num_shaders;
         mgr->render_ctx = render_ctx;

         *manager = mgr;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_shader_manager_destroy(kl_shader_manager_t* manager)
{
   if(manager != NULL)
   {
      kl_shader_manager_t mgr = *manager;
      kl_heap_free(mgr->shader);
      kl_heap_free(mgr);
   }
}

int _get_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader, GLenum shader_type)
{
   int ret = KL_ERROR;
   const char* shader_src = GetShaderSource(KL_DEFAULT_SCRIPT_CONTEXT, effect_key);

   if(shader_src != NULL)
   {
      int compile_success;
      GLuint gl_shader;

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
         skerr(vertexInfoLog);
         kl_heap_free(vertexInfoLog);
      }
      else
      {
         kl_shader_t shdr = kl_heap_alloc(sizeof(struct _kl_shader));
         shdr->shader = gl_shader;
         *shader = shdr;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

int kl_shader_manager_get_vertex_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader)
{
   return _get_shader(render_ctx, effect_key, shader, GL_VERTEX_SHADER);
}

int kl_shader_manager_get_pixel_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader)
{
   return _get_shader(render_ctx, effect_key, shader, GL_FRAGMENT_SHADER);
}

int kl_shader_manager_get_geometry_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader)
{
   return _get_shader(render_ctx, effect_key, shader, GL_GEOMETRY_SHADER_EXT);
}

void kl_shader_manager_destroy_shader(kl_shader_t* shader)
{
   if(shader != NULL)
   {
      kl_shader_t shdr = *shader;
      glDeleteShader(shdr->shader);
      kl_heap_free(shdr);
   }
}
