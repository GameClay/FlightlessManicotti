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

#include <FlightlessManicotti/render/particles/particle_geom_shdr.h>
#include <FlightlessManicotti/math/math.h>
#include <FlightlessManicotti/process/process.h>
#include "render/opengl/gl_render.h"
#include <OpenGL/gl.h>

#include <FlightlessManicotti/beat/beat.h> /* hax */
#include <FlightlessManicotti/render/shader/shader_manager.h>
#include <sanskrit/sklog.h>

struct _kl_particle_render_geom_shdr {
   GLuint vertX_buffer[2];
   GLuint vertY_buffer[2];
   GLuint vertZ_buffer[2];
   GLuint vert_buffer_elements[2];
   GLuint idx_buffer;
   int buffer_idx;
   int last_used_idx;
   kl_particle_system_t system;
   uint32_t pid;
   kl_render_context_t context;

   /* hax */
   GLuint program;
   kl_shader_t vert_shader, pix_shader, geom_shader;
};

void _kl_particle_render_geom_shdr_advance_time(float dt, void* context);

int kl_particle_render_geom_shdr_alloc(kl_particle_render_geom_shdr_t* renderer,
   kl_render_context_t context)
{
   int ret = KL_ERROR;
   if(renderer != NULL)
   {
      kl_particle_render_geom_shdr_t rdr = kl_heap_alloc(sizeof(struct _kl_particle_render_geom_shdr));
      KL_ASSERT(rdr != NULL, "Allocation of geometry shader particle renderer failed.");
      if(rdr != NULL)
      {
         glGenBuffers(2, rdr->vertX_buffer);
         glGenBuffers(2, rdr->vertY_buffer);
         glGenBuffers(2, rdr->vertZ_buffer);
         glGenBuffers(1, &rdr->idx_buffer);
         rdr->buffer_idx = 1;
         rdr->last_used_idx = 1;
         rdr->context = context;
         rdr->vert_buffer_elements[0] = 0;
         rdr->vert_buffer_elements[1] = 0;

         /* TODO: assign other process manager? */
         rdr->pid = kl_reserve_process_id(KL_DEFAULT_PROCESS_MANAGER,
            NULL, &_kl_particle_render_geom_shdr_advance_time, rdr);

         /* Hax */
         {
            kl_shader_manager_get_vertex_shader(context, "ParticleSoA.Vertex.GL2", &rdr->vert_shader);
            kl_shader_manager_get_geometry_shader(context, "ParticleSoA.Geometry.GL2", &rdr->geom_shader);
            kl_shader_manager_get_pixel_shader(context, "ParticleSoA.Fragment.GL2", &rdr->pix_shader);
            rdr->program = glCreateProgram();

            glAttachShader(rdr->program, rdr->vert_shader->shader);
            glAttachShader(rdr->program, rdr->geom_shader->shader);
            glAttachShader(rdr->program, rdr->pix_shader->shader);

            glBindAttribLocation(rdr->program, 0, "in_X");
            glBindAttribLocation(rdr->program, 1, "in_Y");
            glBindAttribLocation(rdr->program, 2, "in_Z");

            glProgramParameteriEXT(rdr->program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
            glProgramParameteriEXT(rdr->program, GL_GEOMETRY_OUTPUT_TYPE_EXT,
               GL_POINTS);
            glProgramParameteriEXT(rdr->program, GL_GEOMETRY_VERTICES_OUT_EXT, 1);

            glLinkProgram(rdr->program);
         }

         *renderer = rdr;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_particle_render_geom_shdr_free(kl_particle_render_geom_shdr_t* renderer)
{
   if(renderer != NULL && *renderer != NULL)
   {
      kl_particle_render_geom_shdr_t rdr = *renderer;

      glDeleteBuffers(2, rdr->vertX_buffer);
      glDeleteBuffers(2, rdr->vertY_buffer);
      glDeleteBuffers(2, rdr->vertZ_buffer);
      glDeleteBuffers(1, &rdr->idx_buffer);

      kl_release_process_id(KL_DEFAULT_PROCESS_MANAGER, rdr->pid);

      /* Hax */
      glDetachShader(rdr->program, rdr->vert_shader->shader);
      glDetachShader(rdr->program, rdr->geom_shader->shader);
      glDetachShader(rdr->program, rdr->pix_shader->shader);
      glDeleteProgram(rdr->program);

      kl_shader_manager_destroy_shader(&rdr->vert_shader);
      kl_shader_manager_destroy_shader(&rdr->pix_shader);

      kl_heap_free(rdr);
   }
}

void kl_particle_render_geom_shdr_assign_system(kl_particle_render_geom_shdr_t renderer,
   kl_particle_system_t system)
{
   if(renderer != NULL)
   {
      renderer->system = system;

      if(system != NULL)
      {
         /* Create static index buffer */
         uint32_t i;
         size_t idx_sz = sizeof(GLushort) * system->max_particles;
         GLushort* indices = kl_heap_alloc(idx_sz);

         for(i = 0; i < system->max_particles; i++)
         {
            indices[i] = i;
         }

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->idx_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_sz, indices, GL_STATIC_DRAW);

         kl_heap_free(indices);
      }
   }
}

void _kl_particle_render_geom_shdr_advance_time(float dt, void* context)
{
   kl_particle_render_geom_shdr_t renderer = (kl_particle_render_geom_shdr_t)context;
   int next_buffer_idx = !renderer->buffer_idx;
   int last_used_idx = renderer->last_used_idx;
   kl_particle_system_t system = renderer->system;
   kl_beat_manager_t beats = kl_beat_manager_default();

   if(system != NULL && last_used_idx != next_buffer_idx)
   {
      uint32_t i = 0;
      uint32_t num_particles = system->num_particles;

      const float* px_stream = system->px_stream;
      const float* py_stream = system->py_stream;
      const float* pz_stream = system->pz_stream;
      const float* vx_stream = system->vx_stream;
      const float* vy_stream = system->vy_stream;
      const float* vz_stream = system->vz_stream;
      const float* lifespan_stream = system->lifespan_stream;
      const float* time_stream = system->time_stream;

      CGLSetCurrentContext(renderer->context->resourceCGLContext);
      CGLLockContext(renderer->context->resourceCGLContext);

      glBindBuffer(GL_ARRAY_BUFFER, renderer->vertX_buffer[next_buffer_idx]);
      glBufferData(GL_ARRAY_BUFFER, num_particles * sizeof(float), px_stream, GL_STREAM_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, renderer->vertY_buffer[next_buffer_idx]);
      glBufferData(GL_ARRAY_BUFFER, num_particles * sizeof(float), py_stream, GL_STREAM_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, renderer->vertZ_buffer[next_buffer_idx]);
      glBufferData(GL_ARRAY_BUFFER, num_particles * sizeof(float), pz_stream, GL_STREAM_DRAW);

      CGLUnlockContext(renderer->context->resourceCGLContext);

      renderer->vert_buffer_elements[next_buffer_idx] = num_particles;

      renderer->buffer_idx = next_buffer_idx;
   }
}

void kl_particle_render_geom_shdr_draw(kl_particle_render_geom_shdr_t renderer)
{
   GLuint buffer_idx = renderer->buffer_idx;

   glUseProgram(renderer->program);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);

   glBindBuffer(GL_ARRAY_BUFFER, renderer->vertX_buffer[buffer_idx]);
   glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
   glBindBuffer(GL_ARRAY_BUFFER, renderer->vertY_buffer[buffer_idx]);
   glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
   glBindBuffer(GL_ARRAY_BUFFER, renderer->vertZ_buffer[buffer_idx]);
   glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->idx_buffer);
   glDrawRangeElements(GL_POINTS, 0, renderer->vert_buffer_elements[buffer_idx],
      renderer->vert_buffer_elements[buffer_idx], GL_UNSIGNED_SHORT, NULL);

   glUseProgram(0);
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);

   renderer->last_used_idx = buffer_idx;
}