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

#include <FlightlessManicotti/render/render.h>
#include <OpenGL/gl.h>
#include "gl_render.h"

/* hax */
#include <FlightlessManicotti/game/particles/particle_system.h>
#include <FlightlessManicotti/render/particles/particle_quads.h>
#include <FlightlessManicotti/game/particles/particle_simulation.h>
kl_particle_system_t haxparticles = NULL;
kl_particle_render_quads_t haxquads = NULL;
kl_particle_simulation_t haxsim = NULL;

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

      /* hax */
      kl_particle_system_alloc(&haxparticles, 2048);
      kl_particle_render_quads_alloc(&haxquads, ctx);
      kl_particle_render_quads_assign_system(haxquads, haxparticles);
      kl_particle_simulation_alloc(&haxsim);
      kl_particle_simulation_set_system(haxsim, haxparticles);

      *context = ctx;
      ret = KL_SUCCESS;
   }

   return ret;
}

void kl_destroy_rendering(kl_render_context_t* context)
{
   /* hax */
   kl_particle_render_quads_free(&haxquads);
   haxquads = NULL;
   kl_particle_system_free(&haxparticles);
   haxparticles = NULL;
   kl_particle_simulation_free(&haxsim);
   haxsim = NULL;

   if(context != NULL && *context != NULL)
   {
      kl_render_context_t ctx = *context;
      CGLReleaseContext(ctx->resourceCGLContext);
      CGLReleaseContext(ctx->drawableCGLContext);
      kl_heap_free(ctx);
   }
}

void kl_render_frame(kl_render_context_t context, float display_width, float display_height)
{
   CGLSetCurrentContext(context->drawableCGLContext);
   CGLLockContext(context->drawableCGLContext);

   glViewport(0, 0, display_width, display_height);

   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);

   glColor3f(1.0f, 0.85f, 0.35f);
   glBegin(GL_TRIANGLES);
   {
      glVertex3f(  0.0,  0.6, 0.0);
      glVertex3f( -0.2, -0.3, 0.0);
      glVertex3f(  0.2, -0.3, 0.0);
   }
   glEnd();

   if(haxquads != NULL && haxparticles != NULL) kl_particle_render_quads_draw(haxquads);

   glFinish();

   CGLUnlockContext(context->drawableCGLContext);
}
