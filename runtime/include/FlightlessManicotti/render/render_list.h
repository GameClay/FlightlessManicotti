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

#ifndef _KL_RENDER_LIST_H_
#define _KL_RENDER_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/math/matrix.h>
#include <FlightlessManicotti/core/idxallocator.h>
#include <FlightlessManicotti/render/mesh/mesh.h>
#include <FlightlessManicotti/render/shader/effect_manager.h>

KL_ALIGNED_STRUCT_PRE(16) {
   kl_matrix_t object_to_world;
   kl_mesh_t* mesh;

   kl_effect_t material; /* TODO: Proper material */
   uint32_t blend_src;
   uint32_t blend_dest;

   kl_shader_constant_t** consts;
   size_t num_consts;

   uint32_t list_index;
   uint32_t draw_type;

   void* render_target;
   KL_BOOL clear_before_draw;
} KL_ALIGNED_STRUCT_POST(kl_render_instance_t, 16);

typedef struct {
   kl_idx_allocator_t idx_alloc;
   kl_render_instance_t** list;
   uint32_t max_idx;
} kl_render_list_t;

/**
 * Initialize a render list.
 *
 * @param list Location to store render list.
 *
 * @return KL_SUCCESS or appropriate error code.
 */
extern KL_API int kl_render_list_init(kl_render_list_t* list, uint32_t list_sz);

/**
 * De-initialize a render list.
 *
 * @param list Render list to de-initialize.
 */
extern KL_API void kl_render_list_deinit(kl_render_list_t* list);

/**
 * Insert a render instance.
 *
 * @param list Render list into which to insert an instance.
 * @param inst Render instance to insert.
 *
 * @return A reserved render instance.
 */
extern KL_API void kl_render_list_insert_instance(kl_render_list_t* list, kl_render_instance_t* inst);

/**
 * Remove a render instance.
 *
 * @param list Render list from which to remove an instance.
 * @param inst Render instance to remove.
 */
extern KL_API void kl_render_list_remove_instance(kl_render_list_t* list, kl_render_instance_t* inst);

#ifdef __cplusplus
}
#endif

#endif
