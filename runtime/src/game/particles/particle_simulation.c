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

#include <FlightlessManicotti/game/particles/particle_simulation.h>

void _kl_particle_simulation_advance_time(float dt, void* context);

extern void kl_particle_simulation_constant(float dt, void* context);

int kl_particle_simulation_alloc(kl_particle_simulation_t* simulation)
{
   int ret = KL_ERROR;

   if(simulation != NULL)
   {
      kl_particle_simulation_t sim = malloc(sizeof(_kl_particle_simulation));
      if(sim != NULL)
      {
         sim->system = NULL;
         sim->advance_fn = &kl_particle_simulation_constant;
         sim->process_mgr = KL_DEFAULT_PROCESS_MANAGER;
         sim->pid = kl_reserve_process_id(sim->process_mgr,
            NULL, &_kl_particle_simulation_advance_time, sim);

         *simulation = sim;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_particle_simulation_free(kl_particle_simulation_t* simulation)
{
   if(simulation != NULL && *simulation != NULL)
   {
      kl_particle_simulation_t sim = *simulation;
      kl_release_process_id(sim->process_mgr, sim->pid);
   }
}

void kl_particle_simulation_set_system(kl_particle_simulation_t simulation, kl_particle_system_t system)
{
   simulation->system = system;
}

void _kl_particle_simulation_advance_time(float dt, void* context)
{
   kl_particle_simulation_t sim = (kl_particle_simulation_t)context;
   sim->advance_fn(dt, context);
}
