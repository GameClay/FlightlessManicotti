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

#include <FlightlessManicotti/render/shader/effect_manager.h>
#include <FlightlessManicotti/core/hash.h>
#include <sanskrit/sklog.h>
#include <stdio.h>
#include <string.h>

#include "render/opengl/gl_render.h"
#include <OpenGL/gl.h>

int kl_effect_manager_create(kl_effect_manager_t* manager, uint32_t num_effects, kl_render_context_t render_ctx)
{
   int ret = KL_ERROR;

   if(manager != NULL)
   {
      kl_effect_manager_t mgr = kl_heap_alloc(sizeof(_kl_effect_manager));
      if(mgr != NULL)
      {
         mgr->effect = kl_heap_alloc(sizeof(struct _kl_effect*) * num_effects);
         kl_zero_mem(mgr->effect, sizeof(struct _kl_effect*) * num_effects);
         mgr->num_effects = num_effects;
         mgr->render_ctx = render_ctx;

         *manager = mgr;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_effect_manager_destroy(kl_effect_manager_t* manager)
{
   if(manager != NULL)
   {
      kl_effect_manager_t mgr = *manager;
      kl_heap_free(mgr->effect);
      kl_heap_free(mgr);
   }
}

int kl_effect_manager_get_effect(kl_render_context_t render_ctx, const char* effect_key,
   kl_effect_t* effect)
{
   int ret = KL_ERROR;
   uint32_t hash = kl_hash(effect_key, strlen(effect_key), 0);
   kl_effect_manager_t mgr = render_ctx->effect_mgr;

   /* Check to see if the effect already exists */
   if(mgr->effect[hash % mgr->num_effects] != NULL)
   {
      kl_effect_t eff = mgr->effect[hash % mgr->num_effects];
      if(strcmp(eff->effect_key, effect_key) == 0)
      {
         *effect = eff;
         eff->ref_count++;
         ret = KL_SUCCESS;
      }
      else
      {
         skerr("Effect key collision, unhandled.");
      }
   }
   else
   {
      char effect_key_buffer[512];
      kl_shader_t vert_shader, geom_shader, pix_shader;
      GLuint program;

      CGLSetCurrentContext(render_ctx->resourceCGLContext);
      CGLLockContext(render_ctx->resourceCGLContext);

      sprintf(effect_key_buffer, "%s.Vertex.%s", effect_key, "GL2");
      if(kl_shader_manager_get_vertex_shader(render_ctx, effect_key_buffer, &vert_shader) == KL_SUCCESS)
      {
         sprintf(effect_key_buffer, "%s.Fragment.%s", effect_key, "GL2");
         if(kl_shader_manager_get_pixel_shader(render_ctx, effect_key_buffer, &pix_shader) == KL_SUCCESS)
         {
            GLint link_success;
            KL_BOOL has_geom = KL_FALSE;
            program = glCreateProgram();

            glAttachShader(program, vert_shader->shader);
            glAttachShader(program, pix_shader->shader);

            sprintf(effect_key_buffer, "%s.Geometry.%s", effect_key, "GL2");
            if(kl_shader_manager_get_geometry_shader(render_ctx, effect_key_buffer, &geom_shader) == KL_SUCCESS)
            {
               glAttachShader(program, geom_shader->shader);

      /* HAX */
      glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
      glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);
      glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, 2);

               has_geom = KL_TRUE;
            }

            glLinkProgram(program);

            /* Check link status */
            glGetProgramiv(program, GL_LINK_STATUS, &link_success);
            if(link_success == GL_TRUE)
            {
               kl_effect_t eff = kl_heap_alloc(sizeof(struct _kl_effect));

               eff->program = program;
               eff->pixel = pix_shader;
               eff->geometry = (has_geom == KL_TRUE ? geom_shader : NULL);
               eff->vertex = vert_shader;
               eff->ref_count = 1;
               strncpy(eff->effect_key, effect_key, KL_SHADER_EFFECT_KEY_SZ);
               mgr->effect[hash % mgr->num_effects] = eff;

               *effect = eff;
               ret = KL_SUCCESS;
            }
            else
            {
               char* linkInfoLog;
               int maxLength;

               glDetachShader(program, vert_shader->shader);
               glDetachShader(program, pix_shader->shader);
               kl_shader_manager_destroy_shader(render_ctx, &pix_shader);
               kl_shader_manager_destroy_shader(render_ctx, &vert_shader);
               glDeleteProgram(program);

               glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
               linkInfoLog = kl_heap_alloc(maxLength);

               glGetProgramInfoLog(program, maxLength, &maxLength, linkInfoLog);
               skerr(linkInfoLog);
               kl_heap_free(linkInfoLog);
            }
         }
         else
         {
            kl_shader_manager_destroy_shader(render_ctx, &vert_shader);
         }
      }

      CGLUnlockContext(render_ctx->resourceCGLContext);
   }

   return ret;
}

void kl_effect_manager_bind_effect(kl_effect_t effect)
{
   if(effect != NULL)
   {
      glUseProgram(effect->program);
   }
   else
   {
      glUseProgram(0);
   }
}

void kl_effect_manager_destroy_effect(kl_render_context_t render_ctx, kl_effect_t* effect)
{
   if(effect != NULL && *effect != NULL)
   {
      kl_effect_t eff = *effect;
      eff->ref_count--;
      if(eff->ref_count == 0)
      {
         uint32_t hash = kl_hash(eff->effect_key, strlen(eff->effect_key), 0);
         render_ctx->effect_mgr->effect[hash % render_ctx->effect_mgr->num_effects] = NULL;
         kl_heap_free(eff);
      }
   }
}
