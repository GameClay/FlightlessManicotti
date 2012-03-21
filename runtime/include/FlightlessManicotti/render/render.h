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

#ifndef _KL_RENDER_H_
#define _KL_RENDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>

typedef struct _kl_render_context* kl_render_context;

/**
 * Initialize rendering subsystem.
 *
 * @param context Location to store rendering context.
 * @param handle  Platform-specific render context handle.
 *
 * @return KL_SUCCESS or appropriate error code.
 */
extern KL_API int kl_init_rendering(kl_render_context* context, void* handle);

/**
 * Destroy the rendering subsystem.
 *
 * @param context Rendering context to destroy.
 */
extern KL_API void kl_destroy_rendering(kl_render_context* context);

/**
 * Indicate that a frame should be rendered.
 *
 * @param context          Render context on which to draw the frame.
 * @param display_width    Width of the frame to render.
 * @param display_height   Height of the frame to render.
 */
extern KL_API void kl_render_frame(kl_render_context context, float display_width, float display_height);

#ifdef __cplusplus
}
#endif

#endif
