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

#ifndef _KL_SCENE_CONTAINER_2D_H_
#define _KL_SCENE_CONTAINER_2D_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"

typedef struct _kl_scene_container_2d_t* kl_scene_container_2d_t;

extern KL_API int kl_alloc_scene_container_2d(kl_scene_container_2d_t* mgr);
extern KL_API void kl_free_scene_container_2d(kl_scene_container_2d_t* mgr);

extern KL_API uint32_t kl_reserve_scene_container_2d_id(kl_scene_container_2d_t mgr);
extern KL_API void kl_free_scene_container_2d_id(kl_scene_container_2d_t mgr, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif
