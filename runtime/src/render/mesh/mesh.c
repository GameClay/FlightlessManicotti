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
         mesh->vertex = kl_heap_alloc(mesh->num_verts * sizeof(float) * 3);
         memcpy(mesh->vertex, verts, mesh->num_verts * sizeof(float) * 3);
         buffer_mask |= kl_mesh_element_vertex;
      }

      normals = ctmGetFloatArray(context, CTM_NORMALS);
      if(normals != NULL)
      {
         mesh->normal = kl_heap_alloc(mesh->num_verts * sizeof(float) * 3);
         memcpy(mesh->normal, normals, mesh->num_verts * sizeof(float) * 3);
         buffer_mask |= kl_mesh_element_normal;
      }

      indices = ctmGetIntegerArray(context, CTM_INDICES);
      if(indices != NULL)
      {
         mesh->index = kl_heap_alloc(mesh->num_indices * sizeof(uint16_t));
         for(i = 0; i < mesh->num_indices; i++)
         {
            mesh->index[i] = indices[i];
         }
         buffer_mask |= kl_mesh_element_index;
      }

      kl_mesh_buffer_data(mesh, buffer_mask, 0);

      ret = KL_SUCCESS;
   }

   ctmFreeContext(context);
   return ret;
}
