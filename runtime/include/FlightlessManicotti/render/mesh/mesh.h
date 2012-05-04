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
#include <stdint.h>

typedef struct _kl_mesh_internal* kl_mesh_internal_t;

typedef struct {
   kl_mesh_internal_t internal;

   float* vertex;
   float* normal;
   float* tex0;
   uint32_t* col0;
   size_t num_verts;

   uint16_t* index;
   size_t num_indices;
} _kl_mesh, *kl_mesh_t;

typedef enum {
   kl_mesh_dynamic_vertex = 1,
   kl_mesh_dynamic_normal = 2,
   kl_mesh_dynamic_tex0   = 4,
   kl_mesh_dynamic_col0   = 8,
   kl_mesh_dynamic_index  = 16
} kl_mesh_dynamic_mask;

/**
 * Initialize an allocated mesh.
 *
 * This function will allocate the mesh internal structure.
 *
 * @note This does not assign any other members of the kl_mesh_t
 *       structure.
 *
 * @param mesh    Mesh to initialize.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_mesh_init(kl_mesh_t mesh);

/**
 * Deinitialize an initialized mesh.
 *
 * This function will deallocate the mesh internal structure,
 * but does not change or modify any other members of the kl_mesh_t
 * structure.
 *
 * @param mesh    Mesh to deinitialize.
 */
extern KL_API void kl_mesh_deinit(kl_mesh_t mesh);

/**
 * Populate the internal buffers of the mesh.
 *
 * This function uses the data contained in the exposed buffers to
 * populate the internal buffers of the mesh.
 *
 * @param mesh          Mesh to populate with internal data.
 * @param dynamic_mask  Mask specifying which mesh elements are dynamic. @see kl_mesh_dynamic_mask
 */
extern KL_API void kl_mesh_buffer_data(kl_mesh_t mesh, uint32_t dynamic_mask);

/**
 * Prepare a mesh for drawing by binding internal buffers.
 *
 * To unbind a mesh, pass NULL for mesh.
 *
 * @param mesh    Mesh to prepare for drawing, or NULL.
 */
extern KL_API void kl_mesh_bind(kl_mesh_t mesh);
