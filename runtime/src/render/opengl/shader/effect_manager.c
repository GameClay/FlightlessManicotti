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
   const char* gl_version_string, kl_effect_t* effect)
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
      float glLanguageVersion;
      GLuint version;

      CGLSetCurrentContext(render_ctx->resourceCGLContext);
      CGLLockContext(render_ctx->resourceCGLContext);

      sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glLanguageVersion);
      version = 100 * glLanguageVersion;
      KL_UNUSED(version);

      sprintf(effect_key_buffer, "%s.Vertex.%s", effect_key, gl_version_string);
      if(kl_shader_manager_get_vertex_shader(render_ctx, effect_key_buffer, &vert_shader) == KL_SUCCESS)
      {
         sprintf(effect_key_buffer, "%s.Fragment.%s", effect_key, gl_version_string);
         if(kl_shader_manager_get_pixel_shader(render_ctx, effect_key_buffer, &pix_shader) == KL_SUCCESS)
         {
            GLint link_success;
            KL_BOOL has_geom = KL_FALSE;
            program = glCreateProgram();

            glAttachShader(program, vert_shader->shader);
            glAttachShader(program, pix_shader->shader);

            sprintf(effect_key_buffer, "%s.Geometry.%s", effect_key, gl_version_string);
            if(kl_shader_manager_get_geometry_shader(render_ctx, effect_key_buffer, &geom_shader) == KL_SUCCESS)
            {
               glAttachShader(program, geom_shader->shader);

               has_geom = KL_TRUE;
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

void kl_effect_manager_bind_effect(kl_effect_t effect, const kl_render_state_t* render_state,
   const kl_shader_constant_t** constant, size_t num_constants)
{
   if(effect != NULL)
   {
      int i;
      int num_tex = 0;
      GLint loc;
      glUseProgram(effect->program);

      /* Assign matrices */
      loc = glGetUniformLocation(effect->program, "object_to_screen");
      if(loc != -1)
      {
         glUniformMatrix4fv(loc, 1, GL_FALSE, render_state->object_to_screen.m);
      }

      /* Assign other constants */
      for(i = 0; i < num_constants; i++)
      {
         loc = glGetUniformLocation(effect->program, constant[i]->name);
         if(loc < 0) continue;

         switch(constant[i]->constant_type)
         {
            case KL_SHADER_CONSTANT_TYPE_MATRIX:
            {
               glUniformMatrix4fv(loc, constant[i]->constant_num,
                  GL_FALSE, constant[i]->constant.as_float_ptr);
               break;
            }

            case KL_SHADER_CONSTANT_TYPE_TEX:
            {
               GLint tex_type;
               glUniform1i(loc, num_tex);
               glActiveTexture(GL_TEXTURE0 + num_tex);
               switch(constant[i]->constant_sz)
               {
                  case 1: tex_type = GL_TEXTURE_1D; break;
                  case 2: tex_type = GL_TEXTURE_2D; break;
                  case 3: tex_type = GL_TEXTURE_3D; break;
                  case 4: tex_type = GL_TEXTURE_CUBE_MAP; break;
               }
               glBindTexture(GL_TEXTURE_2D, constant[i]->constant.as_tex);
               num_tex++;
               break;
            }

            case KL_SHADER_CONSTANT_TYPE_FLOAT:
            {
               switch(constant[i]->constant_sz)
               {
                  case 1:
                  {
                     glUniform1fv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_float_ptr);
                     break;
                  }

                  case 2:
                  {
                     glUniform2fv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_float_ptr);
                     break;
                  }

                  case 3:
                  {
                     glUniform3fv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_float_ptr);
                     break;
                  }

                  case 4:
                  {
                     glUniform4fv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_float_ptr);
                     break;
                  }
               }
               break;
            }

            case KL_SHADER_CONSTANT_TYPE_INT:
            {
               switch(constant[i]->constant_sz)
               {
                  case 1:
                  {
                     glUniform1iv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_int_ptr);
                     break;
                  }

                  case 2:
                  {
                     glUniform2iv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_int_ptr);
                     break;
                  }

                  case 3:
                  {
                     glUniform3iv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_int_ptr);
                     break;
                  }

                  case 4:
                  {
                     glUniform4iv(loc, constant[i]->constant_num,
                        constant[i]->constant.as_int_ptr);
                     break;
                  }
               }
               break;
            }
         }
      }
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
