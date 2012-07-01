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
#include <FreeImage.h>

kl_render_context_t g_script_render_context = NULL;

/* script.events.RenderInit */
kl_script_event_t g_event_RenderInit;

/* script.events.RenderResize */
kl_script_event_t g_event_RenderResize;

static int lua_render_resize_assigner(void* lstate, kl_script_event_t* event)
{
   lua_State* L = lstate;
   kl_render_context_t ctx = (kl_render_context_t)event->event.context.as_ptr;

   lua_newtable(L);

   lua_pushnumber(L, 1);
   lua_pushnumber(L , ctx->display_width);
   lua_rawset(L, -3);

   lua_pushnumber(L, 2);
   lua_pushnumber(L , ctx->display_height);
   lua_rawset(L, -3);

   lua_pushlightuserdata(L, ctx);

   return 2;
}

int kl_init_rendering(kl_render_context_t* context, void* handle)
{
   int ret = KL_ERROR;

   FreeImage_Initialise(FALSE);

   if(context != NULL)
   {
      int i;
      kl_render_context_t ctx = kl_heap_alloc(sizeof(struct _kl_render_context));

      CGLPixelFormatObj pixelFormat = NULL;

      ctx->drawableCGLContext = (CGLContextObj)handle;
      pixelFormat = CGLGetPixelFormat(ctx->drawableCGLContext); /* Not retained, don't need to release */

      CGLRetainContext(ctx->drawableCGLContext);
      CGLCreateContext(pixelFormat, ctx->drawableCGLContext, &ctx->resourceCGLContext);

      CGLSetCurrentContext(ctx->drawableCGLContext);
      CGLLockContext(ctx->drawableCGLContext);

      kl_effect_manager_create(&ctx->effect_mgr);

      /* Null out render list */
      for(i = 0; i < KL_RENDER_CTX_NUM_RENDER_LISTS; i++)
      {
         ctx->render_list[i] = NULL;
      }

      /* Set up render-resize event */
      g_event_RenderResize.event.id = kl_register_script_event("RenderResize");
      g_event_RenderResize.event.context.as_ptr = ctx;
      g_event_RenderResize.event.arg.as_uint32 = 0;
      kl_register_script_event_context_type(g_event_RenderResize.event.id,
         KL_SCRIPT_CONTEXT_TYPE_ASSIGNER, lua_render_resize_assigner);

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

      CGLReleaseContext(ctx->resourceCGLContext);
      CGLReleaseContext(ctx->drawableCGLContext);
      kl_heap_free(ctx);

      FreeImage_DeInitialise();
   }
}

void kl_render_reshape(kl_render_context_t context, float display_width, float display_height)
{
   context->display_width = display_width;
   context->display_height = display_height;
   kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &g_event_RenderResize);
}

KL_BOOL kl_render_assign_list(kl_render_context_t context, kl_render_list_ptr_t list, int list_idx)
{
   KL_BOOL ret = KL_FALSE;
   if(list_idx < KL_RENDER_CTX_NUM_RENDER_LISTS)
   {
      ret = KL_TRUE;
      context->render_list[list_idx] = list;
   }
   return ret;
}

void kl_render_frame(kl_render_context_t context, float display_width, float display_height)
{
   int l_idx;
   kl_transform_state_t hax_xfm_state;
   float aspect_ratio = display_width / display_height;
   kl_matrix_identity(hax_xfm_state.world_to_camera.m);

   CGLSetCurrentContext(context->drawableCGLContext);
   CGLLockContext(context->drawableCGLContext);

   glViewport(0, 0, (GLsizei)display_width, (GLsizei)display_height);

   kl_matrix_ortho(hax_xfm_state.camera_to_screen.m, -aspect_ratio, aspect_ratio, -1.0f, 1.0f, -1.0f, 50.0f);

   /* Camera * Projection */
   kl_matrix_mul_matrix(
      hax_xfm_state.world_to_camera.m,
      hax_xfm_state.camera_to_screen.m,
      hax_xfm_state.world_to_screen.m);

   kl_matrix_mul_matrix(
      KL_MATRIX_IDENTITY.m,
      hax_xfm_state.world_to_screen.m,
      hax_xfm_state.object_to_screen.m);

   /* Clear backbuffer */
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDrawBuffer(GL_COLOR_ATTACHMENT0);
   glViewport(0, 0, (GLsizei)display_width, (GLsizei)display_height);
   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Draw render lists */
   glEnable(GL_BLEND);

   for(l_idx = 0; l_idx < KL_RENDER_CTX_NUM_RENDER_LISTS; l_idx++)
   {
      kl_render_list_t* render_list = context->render_list[l_idx];

      if(render_list != NULL && render_list->list != NULL)
      {
         int32_t max_idx = render_list->max_idx;
         int i;

         /* Set default target and clear */
         if(render_list->default_target != NULL)
         {
            struct _kl_offscreen_target* target = render_list->default_target;
            glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glViewport(0, 0, target->width, target->height);

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         }

         /* Draw list */
         for(i = 0; i <= max_idx; i++)
         {
            const kl_render_instance_t* inst = render_list->list[i];
            if(inst != NULL && inst->mesh != NULL)
            {
               /* Compute matrix */
               kl_matrix_mul_matrix(
                  inst->object_to_world.m,
                  hax_xfm_state.world_to_screen.m,
                  hax_xfm_state.object_to_screen.m);

               /* Set up blend */
               glBlendFunc(inst->blend_src, inst->blend_dest);

               /* Set up target */
               if(inst->render_target != NULL)
               {
                  struct _kl_offscreen_target* target = inst->render_target;
                  glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);
                  glViewport(0, 0, target->width, target->height);
                  glDrawBuffers(inst->num_draw_buffers, inst->draw_buffers);
               }
               else if(render_list->default_target != NULL)
               {
                  struct _kl_offscreen_target* target = render_list->default_target;
                  glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);
                  glViewport(0, 0, target->width, target->height);
                  glDrawBuffer(GL_COLOR_ATTACHMENT0);
               }
               else
               {
                  glBindFramebuffer(GL_FRAMEBUFFER, 0);
                  glDrawBuffer(GL_COLOR_ATTACHMENT0);
                  glViewport(0, 0, (GLsizei)display_width, (GLsizei)display_height);
               }

               /* Clear if requested */
               if(inst->clear_before_draw)
               {
                  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
               }

               /* Bind mesh and shaders */
               if(inst->effect)
               {
                  /* Hax */
                  if(inst->constant_buffer)
                  {
                     kl_effect_manager_new_bind_effect(context->effect_mgr, inst->effect, &hax_xfm_state,
                        inst->constant_buffer);
                  }
                  else
                  {
                     kl_effect_manager_bind_effect(context->effect_mgr, inst->effect, &hax_xfm_state,
                        inst->consts, inst->num_consts);
                  }
               }

               kl_mesh_bind(inst->mesh);

               /* Draw */
               glDrawElements(inst->draw_type, (GLsizei)inst->mesh->num_indices, GL_UNSIGNED_SHORT, NULL);

               /* Tear down target */
               if(inst->render_target != NULL)
               {
                  if(render_list->default_target != NULL)
                  {
                     struct _kl_offscreen_target* target = render_list->default_target;
                     glBindFramebuffer(GL_FRAMEBUFFER, target->framebuffer);
                     glDrawBuffer(GL_COLOR_ATTACHMENT0);
                     glViewport(0, 0, target->width, target->height);
                  }
                  else
                  {
                     glBindFramebuffer(GL_FRAMEBUFFER, 0);
                     glDrawBuffer(GL_COLOR_ATTACHMENT0);
                     glViewport(0, 0, (GLsizei)display_width, (GLsizei)display_height);
                  }
               }
            }
         }
      }
   }
   glDisable(GL_BLEND);

   CGLFlushDrawable(context->drawableCGLContext);
   CGLUnlockContext(context->drawableCGLContext);
}
