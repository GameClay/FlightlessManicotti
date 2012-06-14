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

#ifndef _KL_RENDER_H_
#define _KL_RENDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/math/matrix.h>

typedef struct _kl_render_context* kl_render_context_t;

KL_ALIGNED_STRUCT_PRE(16) {
   kl_matrix_t world_to_camera;
   kl_matrix_t camera_to_screen;
   kl_matrix_t world_to_screen;

   kl_matrix_t object_to_screen;
} KL_ALIGNED_STRUCT_POST(kl_transform_state_t, 16);

#ifndef _KL_RENDER_LIST_PTR_T_
#define _KL_RENDER_LIST_PTR_T_
typedef struct _kl_render_list* kl_render_list_ptr_t;
#endif

/**
 * Initialize rendering subsystem.
 *
 * @param context Location to store rendering context.
 * @param handle  Platform-specific render context handle.
 *
 * @return KL_SUCCESS or appropriate error code.
 *
 * @relates kl_render_context_t
 */
extern KL_API int kl_init_rendering(kl_render_context_t* context, void* handle);

/**
 * Destroy the rendering subsystem.
 *
 * @param context Rendering context to destroy.
 *
 * @relates kl_render_context_t
 */
extern KL_API void kl_destroy_rendering(kl_render_context_t* context);

/**
 * Update the size of the render context.
 *
 * @relates kl_render_context_t
 */
extern KL_API void kl_render_reshape(kl_render_context_t context, float display_width, float display_height);

/**
 * Indicate that a frame should be rendered.
 *
 * @param context          Render context on which to draw the frame.
 * @param display_width    Width of the frame to render.
 * @param display_height   Height of the frame to render.
 *
 * @relates kl_render_context_t
 */
extern KL_API void kl_render_frame(kl_render_context_t context, float display_width, float display_height);

/**
 * Assign a render list to a render context.
 *
 * @param context   Rendering context to assign a renderlist to.
 * @param list      Render list to assign to context.
 * @param list_idx  Index of render list to assign.
 *
 * @return KL_TRUE if assignment was successful, KL_FALSE if the index was out of range.
 *
 * @relates kl_render_context_t
 */
extern KL_API KL_BOOL kl_render_assign_list(kl_render_context_t context, kl_render_list_ptr_t list,
   int list_idx);

#ifdef __cplusplus
}
#endif

#endif
