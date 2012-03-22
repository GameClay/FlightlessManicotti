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

#include <FlightlessManicotti/game/particles/particle_system.h>
#include <FlightlessManicotti/math/math.h>

int kl_particle_system_alloc(kl_particle_system_t* system, uint32_t max_particles)
{
   int ret = KL_SUCCESS;
   kl_particle_system_t sys = kl_heap_alloc(sizeof(_kl_particle_system));
   sys->px_stream = kl_heap_alloc(sizeof(float) * max_particles);
   sys->py_stream = kl_heap_alloc(sizeof(float) * max_particles);
   sys->pz_stream = kl_heap_alloc(sizeof(float) * max_particles);

   sys->vx_stream = kl_heap_alloc(sizeof(float) * max_particles);
   sys->vy_stream = kl_heap_alloc(sizeof(float) * max_particles);
   sys->vz_stream = kl_heap_alloc(sizeof(float) * max_particles);

   sys->lifespan_stream = kl_heap_alloc(sizeof(float) * max_particles);
   sys->time_stream = kl_heap_alloc(sizeof(float) * max_particles);

   sys->num_particles = 0;
   sys->max_particles = max_particles;

   /* Hax, emit particles */
   {
      int i = 0;
      for(i = 0; i < 600; i++)
      {
         float angle = ((rand() % 100) / 100.0f);
         sys->px_stream[i] = kl_cos(angle * KL_2PI) * 0.5f;
         sys->py_stream[i] = kl_sin(angle * KL_2PI) * 0.5f;
         sys->pz_stream[i] = 0.0f;

         sys->lifespan_stream[i] = 30.0f * 1000.0f;
         sys->time_stream[i] = (25.0f + 5.0f * angle) * 1000.0f;
      }
      sys->num_particles = i;
   }

   *system = sys;
   return ret;
}

void kl_particle_system_free(kl_particle_system_t* system)
{
   if(system && *system)
   {
      kl_particle_system_t sys = *system;
      kl_heap_free(sys->px_stream);
      kl_heap_free(sys->py_stream);
      kl_heap_free(sys->pz_stream);

      kl_heap_free(sys->vx_stream);
      kl_heap_free(sys->vy_stream);
      kl_heap_free(sys->vz_stream);

      kl_heap_free(sys->lifespan_stream);
      kl_heap_free(sys->time_stream);

      kl_heap_free(sys);
   }
}
