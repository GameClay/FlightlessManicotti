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

#ifndef _KL_MESH_H_
#define _KL_MESH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/render/render.h>
#include <FlightlessManicotti/render/shader/effect_manager.h>
#include <stdint.h>

typedef struct _kl_mesh_internal* kl_mesh_internal_t;

/**
 * Defines a Mesh render object.
 */
typedef struct {
   kl_mesh_internal_t internal;  /**< Internal mesh structure, API specific. */

   float* vertex;                /**< Pointer to vertex data. Length: 3 * num_verts. */
   float* normal;                /**< Pointer to normal data. Length: 3 * num_verts. */
   float* tex0;                  /**< Pointer to texture coordinate data. Length: 2 * num_verts. */
   uint32_t* col0;               /**< Pointer to color data. Length: num_verts. */
   size_t num_verts;             /**< Number of vertices. */

   uint16_t* index;              /**< Pointer to index data. Length: num_indices. */
   size_t num_indices;           /**< Number of indices. */

   float* face_normal;           /**< Pointer to computed face-normals. Length: num_indices. */
} kl_mesh_t;

/**
 * The Lua mesh holder object.
 */
typedef struct kl_lua_mesh_t {
   kl_mesh_t* mesh;
   uintptr_t auto_dealloc;
} kl_lua_mesh_t;

typedef enum {
   kl_mesh_element_vertex = 1,
   kl_mesh_element_normal = 2,
   kl_mesh_element_tex0   = 4,
   kl_mesh_element_col0   = 8,
   kl_mesh_element_index  = 16,

   kl_mesh_element_all = 0xFFFF
} kl_mesh_element_mask;

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
 *
 * @relates kl_mesh_t
 */
extern KL_API int kl_mesh_init(kl_mesh_t* mesh);

/**
 * Deinitialize an initialized mesh.
 *
 * This function will deallocate the mesh internal structure,
 * but does not change or modify any other members of the kl_mesh_t
 * structure.
 *
 * @param mesh    Mesh to deinitialize.
 *
 * @relates kl_mesh_t
 */
extern KL_API void kl_mesh_deinit(kl_mesh_t* mesh);

/**
 * Populate the internal buffers of the mesh.
 *
 * This function uses the data contained in the exposed buffers to
 * populate the internal buffers of the mesh.
 *
 * @param mesh          Mesh to populate with internal data.
 * @param update_mask   Mask specifying which mesh elements to buffer. @see kl_mesh_element_mask
 * @param dynamic_mask  Mask specifying which mesh elements are dynamic. @see kl_mesh_element_mask
 *
 * @relates kl_mesh_t
 */
extern KL_API void kl_mesh_buffer_data(kl_mesh_t* mesh, uint32_t update_mask, uint32_t dynamic_mask);

/**
 * Prepare a mesh for drawing by binding internal buffers.
 *
 * Pass NULL to unbind meshes.
 *
 * @param mesh    Mesh to prepare for drawing, or NULL.
 *
 * @relates kl_mesh_t
 */
extern KL_API void kl_mesh_bind(const kl_mesh_t* mesh);

/**
 * Load a CTM mesh into an initialized mesh.
 *
 * @param mesh       Mesh to load CTM into.
 * @param mesh_name  Name of the mesh to load.
 *
 * @return KL_SUCCESS if successful.
 *
 * @relates kl_mesh_t
 */
extern KL_API int kl_mesh_load_ctm(kl_mesh_t* mesh, const char* mesh_name);

/**
 * Re-compute per-vertex normals.
 *
 * @param mesh       Mesh on which to recompute normals.
 * @param start_idx  Starting index for recomputation.
 * @param num_tris   Number of triangles starting at start_idx to recompute normals for, or 0 for all.
 *
 * @relates kl_mesh_t
 */
extern KL_API void kl_mesh_recompute_normals(kl_mesh_t* mesh, uint16_t start_idx, uint16_t num_tris);

#ifdef __cplusplus
}
#endif

#endif
