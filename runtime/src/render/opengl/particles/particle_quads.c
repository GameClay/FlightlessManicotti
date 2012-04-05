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
#include <FlightlessManicotti/math/math.h>
#include <FlightlessManicotti/process/process.h>
#include "render/opengl/gl_render.h"
#include <OpenGL/gl.h>

#include <FlightlessManicotti/beat/beat.h> /* hax */
#include <FlightlessManicotti/render/shader/shader_manager.h>
#include <sanskrit/sklog.h>

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

   /* hax */
   GLuint program;
   kl_shader_t vert_shader, pix_shader;
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

         /* Hax */
         {
            kl_shader_manager_get_vertex_shader(context, "PassThrough.Vertex.GL2", &rdr->vert_shader);
            kl_shader_manager_get_pixel_shader(context, "PassThrough.Fragment.GL2", &rdr->pix_shader);
            rdr->program = glCreateProgram();

            glAttachShader(rdr->program, rdr->vert_shader->shader);
            glAttachShader(rdr->program, rdr->pix_shader->shader);

            /* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
            /* Attribute locations must be setup before calling glLinkProgram. */
            glBindAttribLocation(rdr->program, 0, "in_Position");
            glBindAttribLocation(rdr->program, 1, "in_Color");

            glLinkProgram(rdr->program);
         }

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

      /* Hax */
      glDetachShader(rdr->program, rdr->vert_shader->shader);
      glDetachShader(rdr->program, rdr->pix_shader->shader);
      glDeleteProgram(rdr->program);

      kl_shader_manager_destroy_shader(&rdr->vert_shader);
      kl_shader_manager_destroy_shader(&rdr->pix_shader);

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
   kl_beat_manager_t beats = kl_beat_manager_default();

   if(system != NULL && last_used_idx != next_buffer_idx)
   {
      uint32_t i = 0;
      uint32_t num_particles = system->num_particles;
      particle_vertex* verts = renderer->verts;

      const float* px_stream = system->px_stream;
      const float* py_stream = system->py_stream;
      const float* pz_stream = system->pz_stream;
      const float* vx_stream = system->vx_stream;
      const float* vy_stream = system->vy_stream;
      const float* vz_stream = system->vz_stream;
      const float* lifespan_stream = system->lifespan_stream;
      const float* time_stream = system->time_stream;

      /* swap y/z on base points since z is not "up", y is "up" by default */
#define fillVert() { \
      vert->xyz[0] = base_pt->x * size * 0.5f + px_stream[i]; \
      vert->xyz[1] = base_pt->z * size * 0.5f + py_stream[i]; \
      vert->xyz[2] = base_pt->y * size * 0.5f + pz_stream[i]; \
      *((uint32_t*)&vert->color) = *((uint32_t*)&color); }

      for(i = 0; i < num_particles; i++)
      {
         const float t = time_stream[i] / lifespan_stream[i];
         const float size = 0.03f * (kl_cos(beats->beat_interp * KL_PI) + 1.0f) * 0.5f;
         GLubyte color[4] = {0xFF, 0xFF, 0xFF, 0xFF};
         particle_vertex* vert = &verts[i * 4];
         _point3f_simple *base_pt = &sBaseBillboardPoints[i % sNumBillboardPointSets * 4];

         color[1] = (uint8_t)(beats->measure_interp * 0xFF);

         fillVert();
         base_pt++;
         vert++;

         fillVert();
         base_pt++;
         vert++;

         fillVert();
         base_pt++;
         vert++;

         fillVert();
         base_pt++;
         vert++;
      }

#undef fillVert

      CGLSetCurrentContext(renderer->context->resourceCGLContext);
      CGLLockContext(renderer->context->resourceCGLContext);

      glBindBuffer(GL_ARRAY_BUFFER, renderer->vert_buffer[next_buffer_idx]);
      glBufferData(GL_ARRAY_BUFFER, renderer->verts_sz, renderer->verts, GL_STREAM_DRAW);

      CGLUnlockContext(renderer->context->resourceCGLContext);

      renderer->vert_buffer_elements[next_buffer_idx] = num_particles * 2;

      renderer->buffer_idx = next_buffer_idx;
   }
}

void kl_particle_render_quads_draw(kl_particle_render_quads_t renderer)
{
   GLuint buffer_idx = renderer->buffer_idx;

   glUseProgram(renderer->program);

   glBindBuffer(GL_ARRAY_BUFFER, renderer->vert_buffer[buffer_idx]);
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(particle_vertex), (void*)offsetof(particle_vertex, xyz));

   glEnableClientState(GL_COLOR_ARRAY);
   glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(particle_vertex), (void*)offsetof(particle_vertex,color));

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->idx_buffer);
   glDrawRangeElements(GL_TRIANGLES, 0, renderer->vert_buffer_elements[buffer_idx] * 2,
      renderer->vert_buffer_elements[buffer_idx], GL_UNSIGNED_SHORT, NULL);

   glUseProgram(0);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);

   renderer->last_used_idx = buffer_idx;
}
