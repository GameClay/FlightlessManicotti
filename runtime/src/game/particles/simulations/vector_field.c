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
#include <FlightlessManicotti/math/noise.h>

void kl_particle_simulation_vector_field(float dt, void* context)
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

         /* Vector field */
         {
            float* KL_RESTRICT px_stream = system->px_stream;
            float* KL_RESTRICT py_stream = system->py_stream;
            float* KL_RESTRICT pz_stream = system->pz_stream;
            float* KL_RESTRICT vx_stream = system->vx_stream;
            float* KL_RESTRICT vy_stream = system->vy_stream;
            float* KL_RESTRICT vz_stream = system->vz_stream;

            float vx = vx_stream[i];
            float vy = vy_stream[i];
            float vz = vz_stream[i];

            float px = px_stream[i];
            float py = px_stream[i];
            float pz = px_stream[i];

            /* Apply acceleration from vector field */
            /* HAX! Use noise */
            vx += kl_simplex_noise_4d(px, py, pz + new_time, new_time) * dt;
            vy += kl_simplex_noise_4d(px + new_time, py, pz, new_time) * dt;
            vz += kl_simplex_noise_4d(px, py + new_time, pz, new_time) * dt;

            /* Update position */
            px += vx * dt * 0.0001f;
            py += vy * dt * 0.0001f;
            pz += vz * dt * 0.0001f;

            /* Flip velocity if we go off screen */
            vx = kl_fsel(1.0f - fabs(px), vx, -vx);
            vy = kl_fsel(1.0f - fabs(py), vy, -vy);

            /* Store position */
            px_stream[i] = px;
            py_stream[i] = py;
            pz_stream[i] = pz;

            /* Store velocity */
            vx_stream[i] = vx;
            vy_stream[i] = vy;
            vz_stream[i] = vz;
         }
      }
   }
}