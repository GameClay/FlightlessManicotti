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
#include <FlightlessManicotti/core/rarray.h>
#include <FlightlessManicotti/math/vector.h>
#include <openctm.h>
#include <string.h>

int kl_mesh_load_ctm(kl_mesh_t* mesh, const char* mesh_name)
{
   int ret = KL_ERROR;
   CTMcontext context = ctmNewContext(CTM_IMPORT);

   ctmLoad(context, mesh_name);

   if(ctmGetError(context) == CTM_NONE)
   {
      int i;
      const CTMuint* indices;
      const CTMfloat* verts;
      const CTMfloat* normals;
      uint32_t buffer_mask = 0;

      mesh->num_verts = ctmGetInteger(context, CTM_VERTEX_COUNT);
      mesh->num_indices = 3 * ctmGetInteger(context, CTM_TRIANGLE_COUNT);

      verts = ctmGetFloatArray(context, CTM_VERTICES);
      if(verts != NULL)
      {
         if(mesh->vertex != NULL) kl_heap_free(mesh->vertex);
         mesh->vertex = kl_heap_alloc(mesh->num_verts * sizeof(float) * 3);
         memcpy(mesh->vertex, verts, mesh->num_verts * sizeof(float) * 3);
         buffer_mask |= kl_mesh_element_vertex;
      }

      normals = ctmGetFloatArray(context, CTM_NORMALS);
      if(normals != NULL)
      {
         if(mesh->normal != NULL) kl_heap_free(mesh->normal);
         mesh->normal = kl_heap_alloc(mesh->num_verts * sizeof(float) * 3);
         memcpy(mesh->normal, normals, mesh->num_verts * sizeof(float) * 3);
         buffer_mask |= kl_mesh_element_normal;
      }

      indices = ctmGetIntegerArray(context, CTM_INDICES);
      if(indices != NULL)
      {
         if(mesh->index != NULL) kl_heap_free(mesh->index);
         mesh->index = kl_heap_alloc(mesh->num_indices * sizeof(uint16_t));
         for(i = 0; i < mesh->num_indices; i++)
         {
            mesh->index[i] = (uint16_t)indices[i];
         }
         buffer_mask |= kl_mesh_element_index;
      }

      kl_mesh_buffer_data(mesh, buffer_mask, 0);

      ret = KL_SUCCESS;
   }

   ctmFreeContext(context);
   return ret;
}

void kl_mesh_recompute_normals(kl_mesh_t* mesh, uint16_t start_idx, uint16_t num_tris)
{
   if(mesh != NULL && mesh->index != NULL && mesh->vertex != NULL && mesh->normal != NULL &&
      mesh->face_normal != NULL && mesh->num_indices >= num_tris * 3 + start_idx)
   {
      int i, j, k;
      kl_rarray_t* vert_face_assoc;
      kl_vector4_t tmpnrm;

      if(num_tris == 0)
      {
         num_tris = (uint16_t)(mesh->num_indices / 3);
      }

      /* First pass, compute face normals */
      for(i = 0; i < num_tris; i++)
      {
         int face = start_idx / 3 + i;
         float* face_normal = &mesh->face_normal[face * 3];
         const uint16_t* index = &mesh->index[start_idx + i * 3];
         const float* v0 = &mesh->vertex[index[0] * 3];
         const float* v1 = &mesh->vertex[index[1] * 3];
         const float* v2 = &mesh->vertex[index[2] * 3];
         kl_vector4_t vt1;
         kl_vector4_t vt2;
         kl_vector4_t vr;
         float len;

         vt1.v[0] = v1[0] - v0[0];
         vt1.v[1] = v1[1] - v0[1];
         vt1.v[2] = v1[2] - v0[2];
         vt1.v[3] = 0.0f;

         vt2.v[0] = v2[0] - v1[0];
         vt2.v[1] = v2[1] - v1[1];
         vt2.v[2] = v2[2] - v1[2];
         vt2.v[3] = 0.0f;

         kl_vector_cross(vt1.v, vt2.v, vr.v);
         len = sqrtf(vr.v[0] * vr.v[0] + vr.v[1] * vr.v[1] + vr.v[2] * vr.v[2]);
         face_normal[0] = vr.v[0] / len;
         face_normal[1] = vr.v[1] / len;
         face_normal[2] = vr.v[2] / len;
      }

      /* Compute per-vertex normals */
      vert_face_assoc = kl_heap_alloc(sizeof(kl_rarray_t) * mesh->num_verts);
      for(i = 0; i < mesh->num_verts; i++)
      {
         kl_rarray_init(&vert_face_assoc[i], sizeof(uint16_t), 8);
      }

      for(i = 0; i < num_tris; i++)
      {
         const uint16_t* index = &mesh->index[start_idx + i * 3];
         const uint16_t face = (uint16_t)((start_idx / 3) + i);

         kl_rarray_append(&vert_face_assoc[index[0]], &face);
         kl_rarray_append(&vert_face_assoc[index[1]], &face);
         kl_rarray_append(&vert_face_assoc[index[2]], &face);
      }

      for(i = 0; i < num_tris; i++)
      {
         uint16_t idx = (uint16_t)(start_idx + i * 3);

         for(k = 0; k < 3; k++)
         {
            uint16_t index = mesh->index[idx + k];

            const uint16_t* assoc_verts = vert_face_assoc[index].elements;
            float len;
            kl_zero_mem(tmpnrm.v, sizeof(tmpnrm.v));
            for(j = 0; j < vert_face_assoc[index].max_idx; j++)
            {
               int face = assoc_verts[j];
               const float* fnrm = &mesh->face_normal[face * 3];
               tmpnrm.v[0] += fnrm[0];
               tmpnrm.v[1] += fnrm[1];
               tmpnrm.v[2] += fnrm[2];
            }
            tmpnrm.v[0] /= vert_face_assoc[index].max_idx;
            tmpnrm.v[1] /= vert_face_assoc[index].max_idx;
            tmpnrm.v[2] /= vert_face_assoc[index].max_idx;
            len = kl_vector_dot(tmpnrm.v, tmpnrm.v);
            tmpnrm.v[0] /= len;
            tmpnrm.v[1] /= len;
            tmpnrm.v[2] /= len;

            memcpy(&mesh->normal[index * 3], tmpnrm.v, sizeof(float) * 3);
         }
      }

      for(i = 0; i < mesh->num_verts; i++)
      {
         kl_rarray_destroy(&vert_face_assoc[i]);
      }

      kl_heap_free(vert_face_assoc);
   }
}
