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

#ifndef _KL_PARTICLE_RENDER_QUADS_H_
#define _KL_PARTICLE_RENDER_QUADS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/game/particles/particle_system.h>
#include <FlightlessManicotti/render/render.h>

typedef struct _kl_particle_render_quads* kl_particle_render_quads_t;

/**
 * Allocate a quad particle renderer.
 *
 * @param renderer   Location to store allocated particle renderer.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_particle_render_quads_alloc(kl_particle_render_quads_t* renderer, kl_render_context_t context);

/**
 * Free a quad particle renderer.
 *
 * @param renderer   Particle renderer to free.
 */
extern KL_API void kl_particle_render_quads_free(kl_particle_render_quads_t* renderer);

/**
 * Assign a particle system to the quad renderer.
 *
 * @param renderer   Particle renderer to which the system should be assigned.
 * @param system     Particle system to render using this renderer.
 */
extern KL_API void kl_particle_render_quads_assign_system(kl_particle_render_quads_t renderer,
   kl_particle_system_t system);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
