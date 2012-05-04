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

#ifndef _KL_SHADER_MANAGER_H_
#define _KL_SHADER_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/render/render.h>
#include <stdint.h>

typedef struct _kl_shader* kl_shader_t;

typedef struct {
   struct _kl_shader** shader;
   uint32_t num_shaders;
   kl_render_context_t render_ctx;
} _kl_shader_manager, *kl_shader_manager_t;

int kl_shader_manager_create(kl_shader_manager_t* manager, uint32_t num_shaders, kl_render_context_t render_ctx);

void kl_shader_manager_destroy(kl_shader_manager_t* manager);

int kl_shader_manager_get_vertex_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader);

int kl_shader_manager_get_pixel_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader);

int kl_shader_manager_get_geometry_shader(kl_render_context_t render_ctx, const char* effect_key,
   kl_shader_t* shader);

void kl_shader_manager_destroy_shader(kl_render_context_t render_ctx, kl_shader_t* shader);

#ifdef __cplusplus
}
#endif

#endif
