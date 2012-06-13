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

#include <FlightlessManicotti/render/shader/shader_manager.h>
#include <FlightlessManicotti/core/hash.h>
#include "render/script/getshader.h"
#include <stdio.h>
#include <string.h>
#include "render/opengl/gl_render.h"

int kl_shader_manager_create(kl_shader_manager_t* manager, uint32_t num_shaders, kl_render_context_t render_ctx)
{
   int ret = KL_ERROR;

   if(manager != NULL)
   {
      kl_shader_manager_t mgr = kl_heap_alloc(sizeof(_kl_shader_manager));
      if(mgr != NULL)
      {
         mgr->shader = kl_heap_alloc(sizeof(struct _kl_shader*) * num_shaders);
         kl_zero_mem(mgr->shader, sizeof(struct _kl_shader*) * num_shaders);
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
   uint32_t hash = kl_hash(effect_key, strlen(effect_key), shader_type);
   kl_shader_manager_t mgr = render_ctx->shader_mgr;

   /* Check to see if the shader already exists */
   if(mgr->shader[hash % mgr->num_shaders] != NULL)
   {
      kl_shader_t shdr = mgr->shader[hash % mgr->num_shaders];
      if(strcmp(shdr->effect_key, effect_key) == 0)
      {
         *shader = shdr;
         shdr->ref_count++;
         ret = KL_SUCCESS;
      }
      else
      {
         kl_log_err("Shader key collision, unhandled.");
      }
   }
   else
   {
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
            kl_log_err(vertexInfoLog);
            kl_heap_free(vertexInfoLog);

            glDeleteShader(gl_shader);
         }
         else
         {
            kl_shader_t shdr = NULL;

            shdr = kl_heap_alloc(sizeof(struct _kl_shader));
            shdr->shader = gl_shader;
            shdr->ref_count = 1;
            shdr->hash_initial = shader_type;
            strncpy(shdr->effect_key, effect_key, KL_SHADER_EFFECT_KEY_SZ);
            mgr->shader[hash % mgr->num_shaders] = shdr;

            *shader = shdr;
            ret = KL_SUCCESS;
         }
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
   return _get_shader(render_ctx, effect_key, shader, GL_GEOMETRY_SHADER);
}

void kl_shader_manager_destroy_shader(kl_render_context_t render_ctx, kl_shader_t* shader)
{
   if(shader != NULL && *shader != NULL)
   {
      kl_shader_t shdr = *shader;
      shdr->ref_count--;
      if(shdr->ref_count == 0)
      {
         uint32_t hash = kl_hash(shdr->effect_key, strlen(shdr->effect_key), shdr->hash_initial);
         render_ctx->shader_mgr->shader[hash % render_ctx->shader_mgr->num_shaders] = NULL;
         glDeleteShader(shdr->shader);
         kl_heap_free(shdr);
      }
   }
}
