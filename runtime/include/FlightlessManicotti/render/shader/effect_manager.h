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

#include <FlightlessManicotti/render/render.h>
#include <FlightlessManicotti/render/shader/shader_constant.h>

#ifndef _KL_EFFECT_PTR_T_
#define _KL_EFFECT_PTR_T_
typedef struct _kl_effect* kl_effect_ptr_t;
#endif

#define KL_EFFECT_MANAGER_MAX_DATA_SOURCES 512

typedef struct {
   kl_effect_manager_data_source_fn data_source[KL_EFFECT_MANAGER_MAX_DATA_SOURCES];
   const void* data_source_context[KL_EFFECT_MANAGER_MAX_DATA_SOURCES];
} *kl_effect_manager_t, _kl_effect_manager;

extern KL_API int kl_effect_manager_create(kl_effect_manager_t* manager);

extern KL_API void kl_effect_manager_destroy(kl_effect_manager_t* manager);

extern KL_API void kl_effect_manager_register_data_source(kl_render_context_t render_ctx, uint32_t source_id,
   kl_effect_manager_data_source_fn source_fn, const void* context);

extern KL_API void kl_effect_manager_bind_effect(kl_effect_manager_t mgr, kl_effect_ptr_t effect,
   const kl_transform_state_t* xfm_state, kl_shader_constant_t** constant, size_t num_constants);

#ifdef __cplusplus
}
#endif

#endif
