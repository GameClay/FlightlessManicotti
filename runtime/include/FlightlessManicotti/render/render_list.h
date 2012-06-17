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

#ifndef _KL_RENDER_LIST_PTR_T_
#define _KL_RENDER_LIST_PTR_T_
typedef struct _kl_render_list* kl_render_list_ptr_t;
#endif

#ifndef _KL_EFFECT_PTR_T_
#define _KL_EFFECT_PTR_T_
typedef struct _kl_effect* kl_effect_ptr_t;
#endif

KL_ALIGNED_STRUCT_PRE(16) {
   kl_matrix_t object_to_world;     /**< Object-to-world transform */
   kl_mesh_t* mesh;                 /**< Mesh assigned to this render list */
   uint32_t draw_type;              /**< Primitive draw type to use for drawing the associated Mesh. */

   /* TODO: Proper material */
   kl_effect_ptr_t effect;          /**< Effect to use to draw the associated Mesh. */
   uint32_t blend_src;              /**< Source blend to use. */
   uint32_t blend_dest;             /**< Destination blend to use. */

   kl_shader_constant_t** consts;   /**< Shader constants to bind to the associated Material. */
   size_t num_consts;               /**< Number of constants in the consts array. */

   void* render_target;             /**< RenderTarget to which this instance should be drawn, or NULL for the backbuffer */
   uint32_t draw_buffers[15];       /**< Draw buffers. */
   uint32_t num_draw_buffers;       /**< Number of draw buffers. */
   KL_BOOL clear_before_draw;       /**< If set to KL_TRUE, the associated RenderTarget will be cleared before this instance is drawn. */

   kl_render_list_ptr_t list;       /**< The RenderList this instance is associated with. */
   uint32_t list_index;             /**< The index of this instance in the associated RenderList. */
} KL_ALIGNED_STRUCT_POST(kl_render_instance_t, 16);

typedef struct _kl_render_list {
   kl_idx_allocator_t idx_alloc;
   kl_render_instance_t** list;
   uint32_t max_idx;
   void* default_target;
} kl_render_list_t;

/**
 * Initialize a render list.
 *
 * @param list Location to store render list.
 *
 * @return KL_SUCCESS or appropriate error code.
 *
 * @relates kl_render_list_t
 */
extern KL_API int kl_render_list_init(kl_render_list_t* list, uint32_t list_sz);

/**
 * De-initialize a render list.
 *
 * @param list Render list to de-initialize.
 *
 * @relates kl_render_list_t
 */
extern KL_API void kl_render_list_deinit(kl_render_list_t* list);

/**
 * Insert a render instance.
 *
 * @param list Render list into which to insert an instance.
 * @param inst Render instance to insert.
 *
 * @return A reserved render instance.
 *
 * @relates kl_render_list_t
 */
extern KL_API void kl_render_list_insert_instance(kl_render_list_t* list, kl_render_instance_t* inst);

/**
 * Remove a render instance.
 *
 * @param list Render list from which to remove an instance.
 * @param inst Render instance to remove.
 *
 * @relates kl_render_list_t
 */
extern KL_API void kl_render_list_remove_instance(kl_render_list_t* list, kl_render_instance_t* inst);

#ifdef __cplusplus
}
#endif

#endif
