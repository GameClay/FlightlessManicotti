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

#include <FlightlessManicotti/render/mesh/mesh.h>
#include "render/opengl/gl_render.h"

struct _kl_mesh_internal {
   GLuint vert_buffer;
   GLuint norm_buffer;
   GLuint tex0_buffer;
   GLuint col0_buffer;

   GLuint vao;

   GLuint idx_buffer;

   uint32_t buffered_data;
};

int kl_mesh_init(kl_mesh_t* mesh)
{
   int ret = KL_ERROR;

   if(mesh != NULL)
   {
      kl_mesh_internal_t internal = kl_heap_alloc(sizeof(struct _kl_mesh_internal));
      if(internal != NULL)
      {
         glGenVertexArrays(1, &internal->vao);
         glBindVertexArray(internal->vao);
         glGenBuffers(1, &internal->vert_buffer);
         glGenBuffers(1, &internal->norm_buffer);
         glGenBuffers(1, &internal->tex0_buffer);
         glGenBuffers(1, &internal->col0_buffer);
         glGenBuffers(1, &internal->idx_buffer);
         internal->buffered_data = 0;

         mesh->internal = internal;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_mesh_deinit(kl_mesh_t* mesh)
{
   if(mesh != NULL && mesh->internal != NULL)
   {
      glDeleteBuffers(1, &mesh->internal->vert_buffer);
      glDeleteBuffers(1, &mesh->internal->norm_buffer);
      glDeleteBuffers(1, &mesh->internal->tex0_buffer);
      glDeleteBuffers(1, &mesh->internal->col0_buffer);
      glDeleteBuffers(1, &mesh->internal->idx_buffer);
      glDeleteVertexArrays(1, &mesh->internal->vao);

      kl_heap_free(mesh->internal);
      mesh->internal = NULL;
   }
}

#define MESH_DRAW_TYPE(x) ((dynamic_mask & x) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW)

void kl_mesh_buffer_data(kl_mesh_t* mesh, uint32_t update_mask, uint32_t dynamic_mask)
{
   if(mesh != NULL && mesh->internal != NULL)
   {
      kl_mesh_internal_t internal = mesh->internal;
      internal->buffered_data |= update_mask;

      glBindVertexArray(internal->vao);

      if(mesh->vertex != NULL && (update_mask & kl_mesh_element_vertex))
      {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal->vert_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_verts * sizeof(float) * 3,
                      mesh->vertex, MESH_DRAW_TYPE(kl_mesh_element_vertex));
      }

      if(mesh->normal != NULL && (update_mask & kl_mesh_element_normal))
      {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal->norm_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_verts * sizeof(float) * 3,
                      mesh->normal, MESH_DRAW_TYPE(kl_mesh_element_normal));
      }

      if(mesh->tex0 != NULL && (update_mask & kl_mesh_element_tex0))
      {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal->tex0_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_verts * sizeof(float) * 2, /* HAX, should be able to have 1,2,3 or 4 */
                      mesh->tex0, MESH_DRAW_TYPE(kl_mesh_element_tex0));
      }

      if(mesh->col0 != NULL && (update_mask & kl_mesh_element_col0))
      {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal->col0_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_verts * sizeof(uint32_t),
                      mesh->col0, MESH_DRAW_TYPE(kl_mesh_element_col0));
      }

      if(mesh->index != NULL && (update_mask & kl_mesh_element_index))
      {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal->idx_buffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(uint16_t),
                      mesh->index, MESH_DRAW_TYPE(kl_mesh_element_index));
      }
   }
}

#undef MESH_DRAW_TYPE

void kl_mesh_bind(const kl_mesh_t* mesh)
{
   if(mesh != NULL && mesh->internal != NULL)
   {
      kl_mesh_internal_t internal = mesh->internal;

      /* Bind VAO */
      glBindVertexArray(internal->vao);

      if(mesh->vertex != NULL && (internal->buffered_data & kl_mesh_element_vertex))
      {
         glEnableVertexAttribArray(KL_MESH_POSITION_IDX);
         glBindBuffer(GL_ARRAY_BUFFER, internal->vert_buffer);
         glVertexAttribPointer(KL_MESH_POSITION_IDX, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
      }

      if(mesh->normal != NULL && (internal->buffered_data & kl_mesh_element_normal))
      {
         glEnableVertexAttribArray(KL_MESH_NORMAL_IDX);
         glBindBuffer(GL_ARRAY_BUFFER, internal->norm_buffer);
         glVertexAttribPointer(KL_MESH_NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
      }

      if(mesh->tex0 != NULL && (internal->buffered_data & kl_mesh_element_tex0))
      {
         glEnableVertexAttribArray(KL_MESH_TEX0_IDX);
         glBindBuffer(GL_ARRAY_BUFFER, internal->tex0_buffer);
         glVertexAttribPointer(KL_MESH_TEX0_IDX, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
      }

      if(mesh->col0 != NULL && (internal->buffered_data & kl_mesh_element_col0))
      {
         glEnableVertexAttribArray(KL_MESH_COLOR_IDX);
         glBindBuffer(GL_ARRAY_BUFFER, internal->col0_buffer);
         glVertexAttribPointer(KL_MESH_COLOR_IDX, 1, GL_UNSIGNED_INT, GL_TRUE, sizeof(uint32_t), NULL);
      }

      if(mesh->index != NULL && (internal->buffered_data & kl_mesh_element_index))
      {
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal->idx_buffer);
      }
   }
   else
   {
      glDisableVertexAttribArray(KL_MESH_POSITION_IDX);
      glDisableVertexAttribArray(KL_MESH_NORMAL_IDX);
      glDisableVertexAttribArray(KL_MESH_TEX0_IDX);
      glDisableVertexAttribArray(KL_MESH_COLOR_IDX);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   }
}
