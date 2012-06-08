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

#ifndef _KL_PARTICLE_SIMULATION_H_
#define _KL_PARTICLE_SIMULATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/game/particles/particle_system.h>
#include <FlightlessManicotti/process/process.h>

typedef struct {
   uint32_t pid;
   kl_particle_system_t system;
   kl_process_advance_time_ptr advance_fn;
   kl_process_manager_t process_mgr;
} *kl_particle_simulation_t, _kl_particle_simulation;

/**
 * Allocate a particle simulation.
 *
 * @param simulation    Location to store allocated particle simulation.
 *
 * @return KL_SUCCESS if successful.
 *
 * @relates kl_particle_simulation_t
 */
extern KL_API int kl_particle_simulation_alloc(kl_particle_simulation_t* simulation);

/**
 * Free a particle simulation.
 *
 * @param simulation    Particle simulation to free.
 *
 * @relates kl_particle_simulation_t
 */
extern KL_API void kl_particle_simulation_free(kl_particle_simulation_t* simulation);

/**
 * Assign a particle system to a simulation.
 *
 * @param simulation    Particle simulation.
 * @param system        Particle system to assign.
 *
 * @relates kl_particle_simulation_t
 */
extern KL_API void kl_particle_simulation_set_system(kl_particle_simulation_t simulation,
   kl_particle_system_t system);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
