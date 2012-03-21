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

#ifndef _KL_PARTICLE_SYSTEM_H_
#define _KL_PARTICLE_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <stdint.h>

typedef struct {
   float* px_stream;
   float* py_stream;
   float* pz_stream;

   float* vx_stream;
   float* vy_stream;
   float* vz_stream;

   float* lifespan_stream;
   float* time_stream;

   uint32_t num_particles;
   uint32_t max_particles;
} *kl_particle_system_t, _kl_particle_system;

/**
 * Allocate a particle system.
 *
 * @param system          Location to store allocated particle system.
 * @param max_particles   Maximum number of particles in this system.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_particle_system_alloc(kl_particle_system_t* system, uint32_t max_particles);

/**
 * Free a particle system.
 *
 * @param system    Particle system to free.
 */
extern KL_API void kl_particle_system_free(kl_particle_system_t* system);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
