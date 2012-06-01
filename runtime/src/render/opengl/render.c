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
#include <FlightlessManicotti/math/matrix.h>
#include <FlightlessManicotti/scriptinterface/scriptevents.h>
#include <FlightlessManicotti/render/mesh/mesh.h>
#include "scriptinterface/scriptinterface.h"
#include <sanskrit/sklog.h>

kl_render_context_t g_script_render_context = NULL;

kl_script_event_t g_event_RenderInit;

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

}

void kl_render_assign_list(kl_render_context_t context, void* list)
{
   context->render_list = (kl_render_list_t*)list;
}

void kl_render_frame(kl_render_context_t context, float display_width, float display_height)
{
   kl_matrix_t projection_mat;

   CGLSetCurrentContext(context->drawableCGLContext);
   CGLLockContext(context->drawableCGLContext);

   glViewport(0, 0, display_width, display_height);

   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   kl_matrix_ortho(projection_mat.m, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 50.0f);
   glLoadTransposeMatrixf(projection_mat.m);

   /* Draw render list */
   glEnable(GL_BLEND);
   if(context->render_list != NULL && context->render_list->list != NULL)
   {
      uint32_t max_idx = context->render_list->max_idx;
      int i;
      for(i = 0; i <= max_idx; i++)
      {
         kl_render_instance_t* inst = context->render_list->list[i];
         if(inst != NULL && inst->mesh != NULL)
         {
            glMatrixMode(GL_MODELVIEW);
            glLoadTransposeMatrixf(inst->obj_to_world.m);
            glBlendFunc(inst->blend_src, inst->blend_dest);
            kl_mesh_bind(inst->mesh);
            kl_effect_manager_bind_effect(inst->material, inst->consts, inst->num_consts);
            glDrawElements(inst->draw_type, inst->mesh->num_indices, GL_UNSIGNED_SHORT, NULL);
            kl_effect_manager_bind_effect(NULL, NULL, 0);
            kl_mesh_bind(NULL);
         }
      }
   }
   glDisable(GL_BLEND);

   glFinish();

   CGLFlushDrawable(context->drawableCGLContext);
   CGLUnlockContext(context->drawableCGLContext);
}
