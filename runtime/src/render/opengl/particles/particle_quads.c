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

#include <FlightlessManicotti/render/particles/particle_quads.h>
#include <FlightlessManicotti/process/process.h>
#include "render/opengl/gl_render.h"
#include <OpenGL/gl.h>

typedef struct {
   GLfloat xyz[3];
   GLubyte color[4];
   /* UV etc */
} particle_vertex;

struct _kl_particle_render_quads {
   GLuint vert_buffer[2];
   GLuint vert_buffer_elements[2];
   GLuint idx_buffer;
   int buffer_idx;
   int last_used_idx;
   kl_particle_system_t system;
   particle_vertex* verts;
   size_t verts_sz;
   uint32_t pid;
   kl_render_context_t context;
};

void _kl_particle_render_quads_advance_time(float dt, void* context);

int kl_particle_render_quads_alloc(kl_particle_render_quads_t* renderer, kl_render_context_t context)
{
   int ret = KL_ERROR;
   if(renderer != NULL)
   {
      kl_particle_render_quads_t rdr = kl_heap_alloc(sizeof(struct _kl_particle_render_quads));
      KL_ASSERT(rdr != NULL, "Allocation of quad particle renderer failed.");
      if(rdr != NULL)
      {
         glGenBuffers(2, rdr->vert_buffer);
         glGenBuffers(1, &rdr->idx_buffer);
         rdr->buffer_idx = 1;
         rdr->last_used_idx = 1;
         rdr->verts = NULL;
         rdr->context = context;
         rdr->vert_buffer_elements[0] = 0;
         rdr->vert_buffer_elements[1] = 0;

         /* TODO: assign other process manager? */
         rdr->pid = kl_reserve_process_id(KL_DEFAULT_PROCESS_MANAGER,
            NULL, &_kl_particle_render_quads_advance_time, rdr);

         *renderer = rdr;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_particle_render_quads_free(kl_particle_render_quads_t* renderer)
{
   if(renderer != NULL && *renderer != NULL)
   {
      kl_particle_render_quads_t rdr = *renderer;
      glDeleteBuffers(2, rdr->vert_buffer);
      glDeleteBuffers(1, &rdr->idx_buffer);
      kl_heap_free(rdr->verts);
      kl_release_process_id(KL_DEFAULT_PROCESS_MANAGER, rdr->pid);
      kl_heap_free(rdr);
   }
}

void kl_particle_render_quads_assign_system(kl_particle_render_quads_t renderer, kl_particle_system_t system)
{
   if(renderer != NULL)
   {
      renderer->system = system;
      kl_heap_free(renderer->verts);
      renderer->verts = NULL;
      if(system != NULL)
      {
         /* Create static index buffer */
         uint32_t i;
         size_t idx_sz = sizeof(GLushort) * system->max_particles * 6;
         GLushort* indices = kl_heap_alloc(idx_sz);

         for(i = 0; i < system->max_particles; i++)
         {
            GLushort* idx = &indices[i * 6];
            GLushort offset = i * 4; /* if compiler bug, set to volatile */
            idx[0] = 0 + offset;
            idx[1] = 1 + offset;
            idx[2] = 2 + offset;
            idx[3] = 2 + offset;
            idx[4] = 1 + offset;
            idx[5] = 3 + offset;
         }

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->idx_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_sz, indices, GL_STATIC_DRAW);

         kl_heap_free(indices);

         /* Allocate working memory for vertex buffer update */
         renderer->verts_sz = sizeof(particle_vertex) * system->max_particles * 4;
         renderer->verts = kl_heap_alloc(renderer->verts_sz);
      }
   }
}

typedef struct {
   GLfloat x, y, z;
} _point3f_simple;

static _point3f_simple sBaseBillboardPoints[] =
{
   /*
   {-1.0f, 0.0f,-1.0f}, // Bottom Left
   {-1.0f, 0.0f, 1.0f}, // Top Left
   { 1.0f, 0.0f,-1.0f}, // Bottom Right
   { 1.0f, 0.0f, 1.0f}, // Top Right
   */
   {-1.0f, 0.001f,-1.0f},
   {-1.0f, 0.000f, 1.0f},
   { 1.0f, 0.001f,-1.0f},
   { 1.0f, 0.000f, 1.0f},

   {-1.0f,-0.001f,-1.0f},
   {-1.0f, 0.000f, 1.0f},
   { 1.0f,-0.001f,-1.0f},
   { 1.0f, 0.000f, 1.0f},

   {-1.0f, 0.001f,-1.0f},
   {-1.0f, 0.001f, 1.0f},
   { 1.0f, 0.000f,-1.0f},
   { 1.0f, 0.000f, 1.0f},

   {-1.0f,-0.001f,-1.0f},
   {-1.0f,-0.001f, 1.0f},
   { 1.0f, 0.000f,-1.0f},
   { 1.0f, 0.000f, 1.0f},

   {-1.0f, 0.000f,-1.0f},
   {-1.0f, 0.001f, 1.0f},
   { 1.0f, 0.000f,-1.0f},
   { 1.0f, 0.001f, 1.0f},

   {-1.0f, 0.000f,-1.0f},
   {-1.0f,-0.001f, 1.0f},
   { 1.0f, 0.000f,-1.0f},
   { 1.0f,-0.001f, 1.0f},

   {-1.0f, 0.000f,-1.0f},
   {-1.0f, 0.000f, 1.0f},
   { 1.0f, 0.001f,-1.0f},
   { 1.0f, 0.001f, 1.0f},

   {-1.0f, 0.000f,-1.0f},
   {-1.0f, 0.000f, 1.0f},
   { 1.0f,-0.001f,-1.0f},
   { 1.0f,-0.001f, 1.0f},
};
static const int sNumBillboardPointSets = 8;

void _kl_particle_render_quads_advance_time(float dt, void* context)
{
   kl_particle_render_quads_t renderer = (kl_particle_render_quads_t)context;
   int next_buffer_idx = !renderer->buffer_idx;
   int last_used_idx = renderer->last_used_idx;
   kl_particle_system_t system = renderer->system;
   if(system != NULL && last_used_idx != next_buffer_idx)
   {
      uint32_t i = 0;
      uint32_t num_particles = system->num_particles;
      particle_vertex* verts = renderer->verts;

      for(i = 0; i < num_particles; i++)
      {
         verts[i].xyz[0] = system->px_stream[i];
         verts[i].xyz[1] = system->py_stream[i];
         verts[i].xyz[2] = system->pz_stream[i];
      }

      CGLSetCurrentContext(renderer->context->resourceCGLContext);
      CGLLockContext(renderer->context->resourceCGLContext);

      glBindBuffer(GL_ARRAY_BUFFER, renderer->vert_buffer[next_buffer_idx]);
      glBufferData(GL_ARRAY_BUFFER, renderer->verts_sz, renderer->verts, GL_STREAM_DRAW);

      CGLUnlockContext(renderer->context->resourceCGLContext);

      renderer->vert_buffer_elements[next_buffer_idx] = num_particles;

      renderer->buffer_idx = next_buffer_idx;
   }
}

void kl_particle_render_quads_draw(kl_particle_render_quads_t renderer)
{
   GLuint buffer_idx = renderer->buffer_idx;

   glBindBuffer(GL_ARRAY_BUFFER, renderer->vert_buffer[buffer_idx]);
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(particle_vertex), (void*)offsetof(particle_vertex, xyz));

   glDrawArrays(GL_POINTS, 0, renderer->vert_buffer_elements[buffer_idx]);

   renderer->last_used_idx = buffer_idx;
}
