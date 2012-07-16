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
#include <stdio.h>
#include <string.h>

#include <FlightlessManicotti/render/opengl/gl_render.h>

int kl_effect_manager_create(kl_effect_manager_t* manager)
{
   int ret = KL_ERROR;

   if(manager != NULL)
   {
      kl_effect_manager_t mgr = kl_heap_alloc(sizeof(_kl_effect_manager));
      if(mgr != NULL)
      {
         kl_zero_mem(mgr->data_source, sizeof(mgr->data_source));
         kl_zero_mem(mgr->data_source_context, sizeof(mgr->data_source_context));

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
      kl_heap_free(mgr);
   }
}

void kl_effect_manager_register_data_source(kl_render_context_t render_ctx, uint32_t source_id,
   kl_effect_manager_data_source_fn source_fn, const void* context)
{
   kl_effect_manager_t mgr = render_ctx->effect_mgr;

   if(source_id < KL_EFFECT_MANAGER_MAX_DATA_SOURCES)
   {
      mgr->data_source[source_id] = source_fn;
      mgr->data_source_context[source_id] = context;
   }
}

static int _do_constant_assign(kl_effect_manager_t mgr, const kl_shader_constant_t* constant,
   const kl_transform_state_t* xfm_state, int num_tex)
{
   int ret = 0;
   GLint loc = constant->constant_loc;

   switch(constant->constant_type)
   {
      case KL_SHADER_CONSTANT_TYPE_MATRIX:
      {
         const float* m = constant->constant.as_float_ptr;
         switch(constant->data_constant)
         {
            case kl_matrix_data_world_to_camera: m = xfm_state->world_to_camera.m; break;
            case kl_matrix_data_camera_to_screen: m = xfm_state->camera_to_screen.m; break;
            case kl_matrix_data_world_to_screen: m = xfm_state->world_to_screen.m; break;
            case kl_matrix_data_object_to_screen: m = xfm_state->object_to_screen.m; break;
         }
         glUniformMatrix4fv(loc, constant->constant_num,
            GL_FALSE, m);
         break;
      }

      case KL_SHADER_CONSTANT_TYPE_TEX:
      {
         GLint tex_type = GL_TEXTURE_2D;
         glUniform1i(loc, num_tex);
         glActiveTexture(GL_TEXTURE0 + num_tex);
         switch(constant->constant_sz)
         {
            case 1: tex_type = GL_TEXTURE_1D; break;
            case 2: tex_type = GL_TEXTURE_2D; break;
            case 3: tex_type = GL_TEXTURE_3D; break;
            case 4: tex_type = GL_TEXTURE_CUBE_MAP; break;
         }
         glBindTexture(tex_type, constant->constant.as_tex);
         ret += 1;
         break;
      }

      case KL_SHADER_CONSTANT_TYPE_DATA:
      {
         glUniform1i(loc, num_tex);
         glActiveTexture(GL_TEXTURE0 + num_tex);

         if(mgr->data_source[constant->constant.as_tex] != NULL)
         {
            GLint texid = mgr->data_source[constant->constant.as_tex](
               mgr->data_source_context[constant->constant.as_tex]);

            /* HAX! Hard coding 1D texture */
            glBindTexture(GL_TEXTURE_1D, texid);
         }
         ret += 1;
         break;
      }

      case KL_SHADER_CONSTANT_TYPE_FLOAT:
      {
         switch(constant->constant_sz)
         {
            case 1:
            {
               glUniform1fv(loc, constant->constant_num,
                  constant->constant.as_float_ptr);
               break;
            }

            case 2:
            {
               glUniform2fv(loc, constant->constant_num,
                  constant->constant.as_float_ptr);
               break;
            }

            case 3:
            {
               glUniform3fv(loc, constant->constant_num,
                  constant->constant.as_float_ptr);
               break;
            }

            case 4:
            {
               glUniform4fv(loc, constant->constant_num,
                  constant->constant.as_float_ptr);
               break;
            }
         }
         break;
      }

      case KL_SHADER_CONSTANT_TYPE_INT:
      {
         switch(constant->constant_sz)
         {
            case 1:
            {
               glUniform1iv(loc, constant->constant_num,
                  constant->constant.as_int_ptr);
               break;
            }

            case 2:
            {
               glUniform2iv(loc, constant->constant_num,
                  constant->constant.as_int_ptr);
               break;
            }

            case 3:
            {
               glUniform3iv(loc, constant->constant_num,
                  constant->constant.as_int_ptr);
               break;
            }

            case 4:
            {
               glUniform4iv(loc, constant->constant_num,
                  constant->constant.as_int_ptr);
               break;
            }
         }
         break;
      }
   }

   return ret;
}

void kl_effect_manager_bind_effect(kl_effect_manager_t mgr, kl_effect_ptr_t effect,
   const kl_transform_state_t* xfm_state, const kl_shader_constant_buffer_t* constant_buffer)
{
   if(effect != NULL)
   {
      size_t i;
      int num_tex = 0;

      glUseProgram(effect->program);

      /* Assign other constants */
      for(i = 0; i < constant_buffer->num_constants; i++)
      {
#if 0
         kl_shader_constant_t temp_constant;
         if(cur_constant->constant_type == KL_SHADER_CONSTANT_TYPE_FN)
         {
            kl_zero_mem(&temp_constant, sizeof(temp_constant));
            temp_constant.constant_idx = cur_constant->constant_idx;
            cur_constant->constant.as_fn(NULL /* hax */, &temp_constant);
            cur_constant = &temp_constant;
         }
#endif

         num_tex += _do_constant_assign(mgr, &constant_buffer->constant[i],
            xfm_state, num_tex);
      }
   }
   else
   {
      glUseProgram(0);
   }
}
