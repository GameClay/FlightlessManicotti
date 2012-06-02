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

#ifndef _KL_EFFECT_MANAGER_H_
#define _KL_EFFECT_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/render/shader/shader_manager.h>
#include <FlightlessManicotti/render/shader/shader_constant.h>

typedef struct _kl_effect* kl_effect_t;

typedef struct {
   struct _kl_effect** effect;
   uint32_t num_effects;
   kl_render_context_t render_ctx;
} _kl_effect_manager, *kl_effect_manager_t;

extern KL_API int kl_effect_manager_create(kl_effect_manager_t* manager, uint32_t num_effects,
   kl_render_context_t render_ctx);

extern KL_API void kl_effect_manager_destroy(kl_effect_manager_t* manager);

extern KL_API int kl_effect_manager_get_effect(kl_render_context_t render_ctx, const char* effect_key,
   const char* gl_version_string, kl_effect_t* effect);

extern KL_API void kl_effect_manager_bind_effect(kl_effect_t effect, const kl_transform_state_t* xfm_state,
   const kl_shader_constant_t** constants, size_t num_constants);

extern KL_API void kl_effect_manager_destroy_effect(kl_render_context_t render_ctx, kl_effect_t* effect);

#ifdef __cplusplus
}
#endif

#endif
