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
 #include <FlightlessManicotti/render/particles/particle_geom_shdr.h>
#include <FlightlessManicotti/game/particles/particle_simulation.h>
kl_particle_system_t haxparticles = NULL;
kl_particle_render_quads_t haxquads = NULL;
kl_particle_render_geom_shdr_t haxgs = NULL;
kl_particle_simulation_t haxsim = NULL;

/* moar hax */
#include <openctm.h>
GLuint hax_bunny_vert_buffer, hax_bunny_idx_buffer;
CTMuint hax_bunny_vertCount, hax_bunny_triCount;
#include <stdio.h>

/* even moar hax */
#include "scriptinterface/scriptinterface.h"
#include <lauxlib.h>
GLuint hax_lsystem_vert_buffer, hax_lsystem_idx_buffer;
extern kl_script_context_t g_script_context;
int hax_lsystem_line_count;
int hax_lsystem_prim_count = 0;

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

      kl_shader_manager_create(&ctx->shader_mgr, 256, ctx);
      kl_effect_manager_create(&ctx->effect_mgr, 64, ctx);

      /* Create offscreen target */
      ctx->feedback_fbo.width = 512;
      ctx->feedback_fbo.height = 512;
      glGenFramebuffersEXT(1, &ctx->feedback_fbo.framebuffer);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ctx->feedback_fbo.framebuffer);
      glGenTextures(1, &ctx->feedback_fbo.texture);
      glBindTexture(GL_TEXTURE_2D, ctx->feedback_fbo.texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ctx->feedback_fbo.width, ctx->feedback_fbo.height,
                   0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                GL_TEXTURE_2D, ctx->feedback_fbo.texture, 0);

      if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
      {
         /* Blarg! */
      }

      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
      glBindTexture(GL_TEXTURE_2D, 0);

      /* sighhax */
      {
         kl_effect_manager_get_effect(ctx, "PassThroughRowMajor", &hax_effect);
      }

      /* hax */
      kl_particle_system_alloc(&haxparticles, 2048);
      kl_particle_render_quads_alloc(&haxquads, ctx);
      kl_particle_render_quads_assign_system(haxquads, haxparticles);
      kl_particle_render_geom_shdr_alloc(&haxgs, ctx);
      kl_particle_render_geom_shdr_assign_system(haxgs, haxparticles);
      kl_particle_simulation_alloc(&haxsim);
      kl_particle_simulation_set_system(haxsim, haxparticles);

      /* moar hax */
      {
         CTMcontext context;
         const CTMuint * indices;
         const CTMfloat* vertices;

         context = ctmNewContext(CTM_IMPORT);
         ctmLoad(context, "./bunny.ctm");
         if(ctmGetError(context) == CTM_NONE)
         {
            hax_bunny_vertCount = ctmGetInteger(context, CTM_VERTEX_COUNT);
            vertices = ctmGetFloatArray(context, CTM_VERTICES);
            hax_bunny_triCount = ctmGetInteger(context, CTM_TRIANGLE_COUNT);
            indices = ctmGetIntegerArray(context, CTM_INDICES);

            glGenBuffers(1, &hax_bunny_idx_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hax_bunny_idx_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, hax_bunny_triCount * 3 * sizeof(CTMuint),
               indices, GL_STATIC_DRAW);

            glGenBuffers(1, &hax_bunny_vert_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hax_bunny_vert_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, hax_bunny_vertCount * sizeof(CTMfloat) * 3,
               vertices, GL_STATIC_DRAW);
         }

         ctmFreeContext(context);
      }

      /* even moar hax */
      {
         lua_State* L = g_script_context->lua_state;
         lua_getglobal(L, "lsys_verts");
         if(lua_istable(L, -1))
         {
            int i = 0;
            int table_sz = luaL_getn(L, -1);
            float* verts = kl_heap_alloc(sizeof(float) * table_sz);
            float *cvert = verts;
            GLushort* idx;
            lua_pushnil(L);

            while(lua_next(L, -2))
            {
               if(lua_isnumber(L, -1))
               {
                  *cvert = lua_tonumber(L, -1);
                  cvert++;
               }
               lua_pop(L, 1);
            }
            lua_pop(L, 1);

            glGenBuffers(1, &hax_lsystem_vert_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hax_lsystem_vert_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, table_sz * sizeof(float),
               verts, GL_STATIC_DRAW);

            hax_lsystem_line_count = (table_sz / 2) - (table_sz % 2 ? 1 : 0);
            idx = kl_heap_alloc(sizeof(GLushort) * hax_lsystem_line_count);
            for(i = 0; i < hax_lsystem_line_count; i++)
            {
               idx[i] = i;
            }

            glGenBuffers(1, &hax_lsystem_idx_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hax_lsystem_idx_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, hax_lsystem_line_count * sizeof(GLushort),
               idx, GL_STATIC_DRAW);

            kl_heap_free(verts);
            kl_heap_free(idx);
         }
      }

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
   kl_particle_render_geom_shdr_free(&haxgs);
   haxgs = NULL;
   kl_particle_system_free(&haxparticles);
   haxparticles = NULL;
   kl_particle_simulation_free(&haxsim);
   haxsim = NULL;

   /* moar hax */
   glDeleteBuffers(1, &hax_bunny_vert_buffer);
   glDeleteBuffers(1, &hax_bunny_idx_buffer);

   /* even moar hax */
   glDeleteBuffers(1, &hax_lsystem_vert_buffer);
   glDeleteBuffers(1, &hax_lsystem_idx_buffer);

   if(context != NULL && *context != NULL)
   {
      kl_render_context_t ctx = *context;

      glDeleteFramebuffersEXT(1, &ctx->feedback_fbo.framebuffer);
      glDeleteTextures(1, &ctx->feedback_fbo.texture);

      kl_effect_manager_destroy(&ctx->effect_mgr);
      kl_shader_manager_destroy(&ctx->shader_mgr);

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

/*
   if(haxquads != NULL && haxparticles != NULL) kl_particle_render_quads_draw(haxquads);
   */

   if(haxgs != NULL && haxparticles != NULL) kl_particle_render_geom_shdr_draw(haxgs);

   /* moar hax */
   glColor3f(1.0f, 0.85f, 0.35f);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   /*glScalef(3.01f, 3.01f, 3.01f);
   
   glBindBuffer(GL_ARRAY_BUFFER, hax_bunny_vert_buffer);
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(float) * 3, NULL);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hax_bunny_idx_buffer);
   glDrawElements(GL_TRIANGLES, hax_bunny_triCount, GL_UNSIGNED_INT, NULL);
   */
   glColor3f(1.0f, 0.85f, 0.35f);

   glBindBuffer(GL_ARRAY_BUFFER, hax_lsystem_vert_buffer);
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(2, GL_FLOAT, sizeof(float) * 2, NULL);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hax_lsystem_idx_buffer);
   glDrawElements(GL_LINE_STRIP, hax_lsystem_prim_count, GL_UNSIGNED_SHORT, NULL);
   hax_lsystem_prim_count += 20;
   hax_lsystem_prim_count = (hax_lsystem_prim_count < hax_lsystem_line_count ?
      hax_lsystem_prim_count : hax_lsystem_line_count);

   glPopMatrix();
   glDisableClientState(GL_VERTEX_ARRAY);

   glFinish();

   CGLUnlockContext(context->drawableCGLContext);
}
