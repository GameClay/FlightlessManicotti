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

#include <FlightlessManicotti/render/render.h>
#include "gl_render.h"
#include <FlightlessManicotti/scriptinterface/scriptevents.h>
#include <FlightlessManicotti/render/mesh/mesh.h>
#include "scriptinterface/scriptinterface.h"
#include <sanskrit/sklog.h>
#include <FreeImage.h>

int kl_render_spectrum_setting = 0;
kl_render_context_t g_script_render_context = NULL;

/* script.events.RenderInit */
kl_script_event_t g_event_RenderInit;

/* Vizthingy only */
#include <FlightlessManicotti/beat/freq.h>

int kl_init_rendering(kl_render_context_t* context, void* handle)
{
   int ret = KL_ERROR;

   FreeImage_Initialise(FALSE);

   if(context != NULL)
   {
      kl_render_context_t ctx = kl_heap_alloc(sizeof(struct _kl_render_context));

      CGLPixelFormatObj pixelFormat = NULL;

      ctx->drawableCGLContext = (CGLContextObj)handle;
      pixelFormat = CGLGetPixelFormat(ctx->drawableCGLContext); /* Not retained, don't need to release */

      CGLRetainContext(ctx->drawableCGLContext);
      CGLCreateContext(pixelFormat, ctx->drawableCGLContext, &ctx->resourceCGLContext);

      CGLSetCurrentContext(ctx->drawableCGLContext);
      CGLLockContext(ctx->drawableCGLContext);

      kl_shader_manager_create(&ctx->shader_mgr, 256, ctx);
      kl_effect_manager_create(&ctx->effect_mgr, 64, ctx);

      /* Null out render list */
      ctx->render_list = NULL;

      /* Send renderer to freq mgr */
      kl_freq_manager_set_render_ctx(NULL, ctx);

      /* Set default script render context. */
      g_script_render_context = ctx;

      /* Send script event */
      g_event_RenderInit.event.id = kl_register_script_event("RenderInit");
      g_event_RenderInit.event.context.as_ptr = ctx;
      g_event_RenderInit.event.arg.as_uint32 = 0;
      kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &g_event_RenderInit);

      CGLUnlockContext(ctx->drawableCGLContext);

      *context = ctx;
      ret = KL_SUCCESS;
   }

   return ret;
}

void kl_destroy_rendering(kl_render_context_t* context)
{
   if(context != NULL && *context != NULL)
   {
      kl_render_context_t ctx = *context;

      kl_effect_manager_destroy(&ctx->effect_mgr);
      kl_shader_manager_destroy(&ctx->shader_mgr);

      CGLReleaseContext(ctx->resourceCGLContext);
      CGLReleaseContext(ctx->drawableCGLContext);
      kl_heap_free(ctx);

      FreeImage_DeInitialise();
   }
}

void kl_render_reshape(kl_render_context_t context, float display_width, float display_height)
{
   CGLSetCurrentContext(context->drawableCGLContext);
   CGLLockContext(context->drawableCGLContext);

   /* Send script event */

   CGLUnlockContext(context->drawableCGLContext);
}

void kl_render_assign_list(kl_render_context_t context, void* list)
{
   context->render_list = (kl_render_list_t*)list;
}

void kl_render_frame(kl_render_context_t context, float display_width, float display_height)
{
   kl_transform_state_t hax_xfm_state;
   kl_matrix_identity(hax_xfm_state.world_to_camera.m);

   CGLSetCurrentContext(context->drawableCGLContext);
   CGLLockContext(context->drawableCGLContext);

   glViewport(0, 0, display_width, display_height);

   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   kl_matrix_ortho(hax_xfm_state.camera_to_screen.m, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 50.0f);

   /* Camera * Projection */
   kl_matrix_mul_matrix(
      hax_xfm_state.world_to_camera.m,
      hax_xfm_state.camera_to_screen.m,
      hax_xfm_state.world_to_screen.m);

   kl_matrix_mul_matrix(
      KL_MATRIX_IDENTITY.m,
      hax_xfm_state.world_to_screen.m,
      hax_xfm_state.object_to_screen.m);

   /* Draw render list */
   /* Hax */
   glBindTexture(GL_TEXTURE_1D, kl_freq_manager_get_cqt_texture(NULL));
   /* /Hax */

   glEnable(GL_BLEND);
   if(context->render_list != NULL && context->render_list->list != NULL)
   {
      uint32_t max_idx = context->render_list->max_idx;
      int i;
      for(i = 0; i <= max_idx; i++)
      {
         const kl_render_instance_t* inst = context->render_list->list[i];
         if(inst != NULL && inst->mesh != NULL)
         {
            /* Compute matrix */
            /*
            kl_matrix_mul_matrix(
               inst->object_to_world.m,
               hax_render_state.world_to_screen.m,
               hax_render_state.object_to_screen.m);*/

            /* Set up blend */
            glBlendFunc(inst->blend_src, inst->blend_dest);

            /* Set up target */
            if(inst->render_target != NULL)
            {
               struct _kl_offscreen_target* target = inst->render_target;
               glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);
               glViewport(0, 0, target->width, target->height);
            }

            /* Clear if requested */
            if(inst->clear_before_draw)
            {
               glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
               glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            /* Bind mesh and shaders */
            kl_effect_manager_bind_effect(inst->material, &hax_xfm_state,
               (const kl_shader_constant_t**)inst->consts, inst->num_consts);
            kl_mesh_bind(inst->mesh);

            /* Draw */
            glDrawElements(inst->draw_type, inst->mesh->num_indices, GL_UNSIGNED_SHORT, NULL);

            /* Unbind shaders and mesh */
            kl_mesh_bind(NULL);
            kl_effect_manager_bind_effect(NULL, NULL, NULL, 0);

            /* Tear down target */
            if(inst->render_target != NULL)
            {
               glBindFramebuffer(GL_FRAMEBUFFER, 0);
               glViewport(0, 0, display_width, display_height);
            }
         }
      }
   }
   glDisable(GL_BLEND);

#if 0
   {
      float cqt_colors[12][3] = {
         {1.0f, 0.0f, 0.0f},  /* C */
         {1.0f, 0.5f, 0.0f},  /* C#/Db */
         {1.0f, 1.0f, 0.0f},  /* D */
         {0.5f, 1.0f, 0.0f},  /* D#/Eb */
         {0.0f, 1.0f, 0.0f},  /* E */
         {0.0f, 1.0f, 0.5f},  /* F */
         {0.0f, 1.0f, 1.0f},  /* F#/Gb */
         {0.0f, 0.5f, 1.0f},  /* G */
         {0.0f, 0.0f, 1.0f},  /* G#/Ab */
         {0.5f, 0.0f, 1.0f},  /* A */
         {1.0f, 0.0f, 1.0f},  /* A#/Bb */
         {1.0f, 0.0f, 0.5f},  /* B */
      };
      kl_cqt_t cqt = kl_freq_manager_get_cqt(NULL);

      /* Spectrum/wave visualization*/
      if(kl_render_spectrum_setting == 0)
      {
         /* Render FFT spectrum */
         int i;
         size_t spectrum_sz;
         float* hax_spectrum = kl_freq_manager_get_spectrum(KL_DEFAULT_FREQ_MANAGER, &spectrum_sz);
         glBegin(GL_LINES);
         for(i = 0; i < spectrum_sz; i++)
         {
            float interp = (float)i / spectrum_sz;
            glColor3f(1.0f - interp, interp, 0.0f);
            glVertex2f(-1.0f + interp * 2.0f, -0.99f);
            glVertex2f(-1.0f + interp * 2.0f, hax_spectrum[i] - 0.99f);
         }
         glEnd();
      }
      else if(kl_render_spectrum_setting == 1)
      {
         /* Render CQT spectrum */
         int i, o, num_octaves, bins_per_octave;
         float** octaves;

         if(kl_freq_manager_get_cqt_texture(NULL) != 0) /* HAX */
         {
            kl_cqt_get_octaves(cqt, &octaves, &num_octaves, &bins_per_octave);
            glBegin(GL_LINES);
            for(o = 0; o < num_octaves; o++)
            {
               for(i = 0; i < bins_per_octave; i++)
               {
                  float val = octaves[o][i];
                  float interp = (float)(o * bins_per_octave + i) / (float)(num_octaves * bins_per_octave);
                  int cindex = floor(12.0f * i / bins_per_octave);
                  glColor3fv(cqt_colors[cindex]);
                  glVertex2f(-1.0f + interp * 2.0f, -0.99f);
                  glVertex2f(-1.0f + interp * 2.0f, val - 0.99f);
               }
            }
            glEnd();
         }
      }
      else if(kl_render_spectrum_setting == 2)
      {
         /* Render CQT waves on top of CQT spectrum */
         if(kl_freq_manager_get_cqt_texture(NULL) != 0) /* HAX */
         {
            int i, o, num_octaves, bins_per_octave;
            float** octaves;
            int num_waves;
            kl_cqt_wave_t* waves = kl_cqt_get_waves(cqt, &num_waves);
            kl_cqt_get_octaves(cqt, &octaves, &num_octaves, &bins_per_octave);

            glBegin(GL_LINES);
            for(o = 0; o < num_octaves; o++)
            {
               for(i = 0; i < bins_per_octave; i++)
               {
                  float val = octaves[o][i];
                  float interp = (float)(o * bins_per_octave + i) / (float)(num_octaves * bins_per_octave);
                  int cindex = floor(12.0f * i / bins_per_octave);
                  glColor3f(cqt_colors[cindex][0] * 0.6f,
                            cqt_colors[cindex][1] * 0.6f,
                            cqt_colors[cindex][2] * 0.6f);
                  glVertex2f(-1.0f + interp * 2.0f, -0.99f);
                  glVertex2f(-1.0f + interp * 2.0f, val - 0.99f);
               }
            }
            glEnd();

            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            for(i = 0; i < num_waves; i++)
            {
               if(waves[i].amp_state != KL_WAVE_STATE_DISSIPATED)
               {
                  glVertex2f(-1.0f + ((float)waves[i].peak / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                  glVertex2f(-1.0f + ((float)waves[i].peak / (float)(num_octaves * bins_per_octave)) * 2.0f, waves[i].peakval - 0.99f);

                  glVertex2f(-1.0f + ((float)waves[i].start / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                  glVertex2f(-1.0f + ((float)waves[i].end   / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
               }
            }
            glEnd();
         }
      }
      else if(kl_render_spectrum_setting == 3)
      {
         /* Render CQT waves */
         if(kl_freq_manager_get_cqt_texture(NULL) != 0) /* HAX */
         {
            int i, num_octaves, bins_per_octave;
            float** octaves;
            int num_waves;
            kl_cqt_wave_t* waves = kl_cqt_get_waves(cqt, &num_waves);
            kl_cqt_get_octaves(cqt, &octaves, &num_octaves, &bins_per_octave);
            for(i = 0; i < num_waves; i++)
            {
               if(waves[i].amp_state != KL_WAVE_STATE_DISSIPATED)
               {
                  int peak_color = ((float)(waves[i].peak % bins_per_octave)) / (float)bins_per_octave * 12;
                  int start_color = ((float)(waves[i].start % bins_per_octave)) / (float)bins_per_octave * 12;
                  int end_color = ((float)(waves[i].end % bins_per_octave)) / (float)bins_per_octave * 12;

                  glBegin(GL_LINES);
                     glColor3fv(cqt_colors[peak_color]);
                     glVertex2f(-1.0f + ((float)waves[i].peak / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                     glVertex2f(-1.0f + ((float)waves[i].peak / (float)(num_octaves * bins_per_octave)) * 2.0f, waves[i].peakval - 0.99f);

                     glColor3fv(cqt_colors[start_color]);
                     glVertex2f(-1.0f + ((float)waves[i].start / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                     glColor3fv(cqt_colors[end_color]);
                     glVertex2f(-1.0f + ((float)waves[i].end   / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                  glEnd();
               }
            }
         }
      }
      else if(kl_render_spectrum_setting == 4)
      {
         /* Render CQT waves */
         if(kl_freq_manager_get_cqt_texture(NULL) != 0) /* HAX */
         {
            int i, o, num_octaves, bins_per_octave;
            float** octaves;
            int num_waves;
            kl_cqt_wave_t* waves = kl_cqt_get_waves(cqt, &num_waves);

            float state_colors[5][3] = {
               {0.0f, 0.0f, 0.0f},  /* Dissipating */
               {1.0f, 1.0f, 1.0f},  /* New */
               {0.0f, 1.0f, 0.0f},  /* Increasing */
               {1.0f, 0.0f, 0.0f},  /* Decreasing */
               {0.0f, 0.0f, 1.0f},  /* Constant */
            };

            kl_cqt_get_octaves(cqt, &octaves, &num_octaves, &bins_per_octave);

            glBegin(GL_LINES);
            for(o = 0; o < num_octaves; o++)
            {
               for(i = 0; i < bins_per_octave; i++)
               {
                  float val = octaves[o][i];
                  float interp = (float)(o * bins_per_octave + i) / (float)(num_octaves * bins_per_octave);
                  glColor3f(0.4f, 0.4f, 0.4f);
                  glVertex2f(-1.0f + interp * 2.0f, -0.99f);
                  glVertex2f(-1.0f + interp * 2.0f, val - 0.99f);
               }
            }
            glEnd();

            for(i = 0; i < num_waves; i++)
            {
               if(waves[i].amp_state != KL_WAVE_STATE_DISSIPATED)
               {
                  glBegin(GL_LINES);
                     glColor3fv(state_colors[waves[i].amp_state]);
                     glVertex2f(-1.0f + ((float)waves[i].peak / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                     glVertex2f(-1.0f + ((float)waves[i].peak / (float)(num_octaves * bins_per_octave)) * 2.0f, waves[i].peakval - 0.99f);

                     glColor3fv(state_colors[waves[i].pitch_state]);
                     glVertex2f(-1.0f + ((float)waves[i].start / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                     glVertex2f(-1.0f + ((float)waves[i].end   / (float)(num_octaves * bins_per_octave)) * 2.0f, -0.99f);
                  glEnd();
               }
            }
         }
      }
   }
#endif

   glFinish();

   CGLFlushDrawable(context->drawableCGLContext);
   CGLUnlockContext(context->drawableCGLContext);
}
