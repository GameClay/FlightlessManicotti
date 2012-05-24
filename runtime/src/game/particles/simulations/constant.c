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
#include <FlightlessManicotti/math/math.h>

void kl_particle_simulation_constant(float dt, void* context)
{
   kl_particle_simulation_t sim = (kl_particle_simulation_t)context;
   kl_particle_system_t system = sim->system;

   if(system != NULL)
   {
      int i = 0;
      float* time_stream = system->time_stream;
      for(i = 0; i < system->num_particles; i++)
      {
         float new_time = time_stream[i] - dt;
         if(time_stream[i] <= 0.0f)
         {
            int last_idx = system->num_particles - 1;
            if(i < last_idx)
            {
               system->px_stream[i] = system->px_stream[last_idx];
               system->py_stream[i] = system->py_stream[last_idx];
               system->pz_stream[i] = system->pz_stream[last_idx];

               system->vx_stream[i] = system->vx_stream[last_idx];
               system->vy_stream[i] = system->vy_stream[last_idx];
               system->vz_stream[i] = system->vz_stream[last_idx];

               system->lifespan_stream[i] = system->lifespan_stream[last_idx];
               time_stream[i] = time_stream[last_idx];

               i--;
            }

            system->num_particles--;
            continue;
         }

         time_stream[i] = kl_fsel(new_time, new_time, 0.0f);
      }
   }
}