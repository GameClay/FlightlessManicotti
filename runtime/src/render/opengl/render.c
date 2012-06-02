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

int kl_render_spectrum_setting = 0;
kl_render_context_t g_script_render_context = NULL;

/* script.events.RenderInit */
kl_script_event_t g_event_RenderInit;

/* heinous hax */
#include <FlightlessManicotti/beat/freq.h>

/* offensive hax */
kl_mesh_t* g_hax_script_mesh = NULL;

int kl_init_rendering(kl_render_context_t* context, void* handle)
{
   int ret = KL_ERROR;

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

/* Slight hax */
extern void kl_cqt_lights_enable(kl_cqt_t cqt);
extern void kl_cqt_lights_disable();

void kl_render_frame(kl_render_context_t context, float display_width, float display_height)
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

   kl_matrix_t projection_mat;
   kl_matrix_t modelview_mat;

   CGLSetCurrentContext(context->drawableCGLContext);
   CGLLockContext(context->drawableCGLContext);

   glViewport(0, 0, display_width, display_height);

   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   kl_matrix_ortho(projection_mat.m, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 50.0f);
   glLoadTransposeMatrixf(projection_mat.m);

#if 1
   {
      float cqt_spectrum[4096];
      int i, o, num_octaves, bins_per_octave;
      float** octaves;

      if(kl_freq_manager_get_cqt_texture(NULL) != 0) /* HAX */
      {
         int active_target = context->active_fbo;

         glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, context->feedback_fbo[active_target].framebuffer);
         glViewport(0, 0, context->feedback_fbo[active_target].width,
                    context->feedback_fbo[active_target].height);

         glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         glMatrixMode(GL_MODELVIEW);
         glLoadTransposeMatrixf(KL_MATRIX_IDENTITY.m);

         /* render to feedback buffer */
         {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, context->feedback_fbo[!active_target].texture);

            glBegin(GL_TRIANGLE_STRIP);
               glColor4f(0.995f, 0.995f, 0.995f, 1.0f);

               glTexCoord2f(0.0f, 0.0f);
               glVertex2f(-1.01f, -1.01f);

               glTexCoord2f(0.0f, 1.0f);
               glVertex2f(-1.01f, 1.01f);

               glTexCoord2f(1.0f, 0.0f);
               glVertex2f(1.01f, -1.01f);

               glTexCoord2f(1.0f, 1.0f);
               glVertex2f(1.01f, 1.01f);
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
         }

         kl_zero_mem(cqt_spectrum, sizeof(cqt_spectrum));
         kl_cqt_get_octaves(cqt, &octaves, &num_octaves, &bins_per_octave);
         for(o = 3; o < num_octaves; o++)
         {
            for(i = 0; i < bins_per_octave; i++)
            {
               float val = octaves[o][i];
               cqt_spectrum[i] += val;
            }
         }

         kl_matrix_scale(modelview_mat.m, 0.3f, 0.3f, 0.3f);
         glLoadTransposeMatrixf(modelview_mat.m);

         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         /* Render melody range */
         glBegin(GL_LINE_LOOP);
         for(i = 0; i < bins_per_octave; i++)
         {
            float interp = (float)i / (float)bins_per_octave;
            float angle = interp * KL_2PI_F;
            float vx = -1.0f * kl_cos(angle);
            float vy = kl_sin(angle);
            int cindex = floor(12 * interp);
            float val = cqt_spectrum[i];
            glColor4f(cqt_colors[cindex][0],
                      cqt_colors[cindex][1],
                      cqt_colors[cindex][2], val * 10.0f);
            glVertex2f(vx * (0.9f - val),
                       vy * (0.9f - val));
         }
         glEnd();
         glDisable(GL_BLEND);

         glLoadTransposeMatrixf(KL_MATRIX_IDENTITY.m);

         glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
         glViewport(0, 0, display_width, display_height);

         /* Render result to back buffer */
         {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, context->feedback_fbo[active_target].texture);

            glBegin(GL_TRIANGLE_STRIP);
               glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

               glTexCoord2f(0.0f, 0.0f);
               glVertex2f(-1.0f, -1.0f);

               glTexCoord2f(0.0f, 1.0f);
               glVertex2f(-1.0f, 1.0f);

               glTexCoord2f(1.0f, 0.0f);
               glVertex2f(1.0f, -1.0f);

               glTexCoord2f(1.0f, 1.0f);
               glVertex2f(1.0f, 1.0f);
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
         }

         context->active_fbo = !active_target;
      }
   }
#endif

   /* Draw render list */
   /* Hax */
   glEnable(GL_TEXTURE_1D);
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
            /* Set obj->world matrix */
            glMatrixMode(GL_MODELVIEW);
            glLoadTransposeMatrixf(inst->obj_to_world.m);

            /* Set up blend */
            glBlendFunc(inst->blend_src, inst->blend_dest);

            /* Bind mesh and shaders */
            kl_effect_manager_bind_effect(inst->material, NULL,
               (const kl_shader_constant_t**)inst->consts, inst->num_consts);
            kl_mesh_bind(inst->mesh, inst->material);

            /* Set up target */
            if(inst->render_target != NULL)
            {
               struct _kl_offscreen_target* target = inst->render_target;
               glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, target->framebuffer);
               glViewport(0, 0, target->width, target->height);
            }

            /* Clear if requested */
            if(inst->clear_before_draw)
            {
               glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
               glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            /* Draw */
            glDrawElements(inst->draw_type, inst->mesh->num_indices, GL_UNSIGNED_SHORT, NULL);

            /* Tear down target */
            if(inst->render_target != NULL)
            {
               glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
               glViewport(0, 0, display_width, display_height);
            }

            /* Unbind mesh/shaders */
            kl_mesh_unbind(inst->mesh, inst->material);
            kl_effect_manager_bind_effect(NULL, NULL, NULL, 0);
         }
      }
   }
   glDisable(GL_BLEND);

   glDisable(GL_TEXTURE_1D); /* Hax */

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

#if 1
   if(g_hax_script_mesh != NULL)
   {
      int i;
      kl_mesh_bind(g_hax_script_mesh);

      glMatrixMode(GL_MODELVIEW);
      glLoadTransposeMatrixf(KL_MATRIX_IDENTITY.m);

      glEnable(GL_DEPTH_TEST);

      if(kl_freq_manager_get_cqt_texture(NULL) != 0) /* HAX */
      {
         int o, num_octaves, bins_per_octave;
         float** octaves;

         kl_cqt_lights_enable(cqt);

         kl_cqt_get_octaves(cqt, &octaves, &num_octaves, &bins_per_octave);

         /* bass-based rotation */
         o = (bins_per_octave + bins_per_octave / 2) / 3;
         {
            static float bass_theta_1 = 0.0f;
            static float bass_theta_2 = 0.0f;
            static float bass_theta_3 = 0.0f;

            static float bass_theta_v1 = 0.0f;
            static float bass_theta_v2 = 0.0f;
            static float bass_theta_v3 = 0.0f;

            bass_theta_v1 *= 0.87f;
            bass_theta_v2 *= 0.87f;
            bass_theta_v3 *= 0.87f;

            for(i = 0; i < o * 3; i++)
            {
               if(i < o)
               {
                  bass_theta_v1 += octaves[0][i];
               }
               else if(i < o * 2)
               {
                  bass_theta_v2 += octaves[0][i];
               }
               else
               {
                  bass_theta_v3 += octaves[0][i];
               }
            }

            bass_theta_1 += bass_theta_v1;
            bass_theta_2 += bass_theta_v2;
            bass_theta_3 += bass_theta_v3;

            kl_matrix_scale(modelview_mat.m, 0.15f, 0.15f, 0.15f);
            glLoadTransposeMatrixf(modelview_mat.m);

            glRotatef(bass_theta_1, 1.0f, 0.0f, 0.0f);
            glRotatef(bass_theta_2, 0.0f, 1.0f, 0.0f);
            glRotatef(bass_theta_3, 0.0f, 0.0f, 1.0f);
         }
      }

/*glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );*/
      glDrawElements(GL_TRIANGLES, g_hax_script_mesh->num_indices, GL_UNSIGNED_SHORT, NULL);
/*glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );*/

      kl_cqt_lights_disable();

      glDisable(GL_DEPTH_TEST);

      kl_mesh_bind(NULL);
   }
#endif

   glFinish();

   CGLFlushDrawable(context->drawableCGLContext);
   CGLUnlockContext(context->drawableCGLContext);
}
