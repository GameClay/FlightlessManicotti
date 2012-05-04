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

#ifndef _KL_SCENE_CONTAINER_2D_H_
#define _KL_SCENE_CONTAINER_2D_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/process/process.h>
#include <FlightlessManicotti/core/idxallocator.h>

typedef struct
{
   kl_process_manager_t process_manager;  /**< Process manager on which this scene container is registered. */
   uint32_t pid;                          /**< Process index for this scene container in its process_manager. */
   
   kl_idx_allocator_t id_allocator;       /**< Allocates indices into the scene. */
   uint32_t max_entries;                  /**< Maximum number of entires in the scene. */
   
   uint32_t min_id;                       /**< Minimum id value which is currently reserved. */
   uint32_t max_id;                       /**< Maximum id value which is currently reserved.  */
   
   int* reference;                        /**< Lua reference associated with each entry. Indexed by id. */
   
   uint32_t* typemask;                    /**< Typemasks for entries in the scene. Indexed by id. */
   
   float* pos_xy;                         /**< Positions for entries in the scene. Indexed by id * 2. */
   
   float* radius;                         /**< Circular bounds radius for entires in the scene. Indexed by id. */
   
   float* anchor_xy;                      /**< Anchor point for entires in the scene. Indexed by id * 2. */
   float* rotation;                       /**< Rotation around anchor point, in radians. Indexed by id. */
}*kl_scene_container_2d_t, _kl_scene_container_2d;

enum
{
   SceneType_Reserved         = 1 << 0   /**< This entry has been reserved. */
};

/**
 * Allocate a 2d scene container.
 *
 * @param container       Location to store allocated scene container.
 * @param process_manager Proccess manager on which to sign up to advance-time/process-tick.
 * @param max_entries     Maximum number of objects which can be managed by this container.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_alloc_scene_container_2d(kl_scene_container_2d_t* container, 
   kl_process_manager_t process_manager, uint32_t max_entries);

/**
 * Free a 2d scene container.
 *
 * @param container    Scene container to free.
 */
extern KL_API void kl_free_scene_container_2d(kl_scene_container_2d_t* container);

/**
 * Reserve an index in the scene container.
 *
 * @param container    Scene container from which to reserve an id.
 * @param out_id       Storage location for id, if reservation is successful.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_reserve_scene_container_2d_id(kl_scene_container_2d_t container, uint32_t* out_id);

/**
 * Free a previously reserved scene id.
 *
 * @param container    Scene container on which to free the id.
 * @param id           Id to free.
 */
extern KL_API void kl_free_scene_container_2d_id(kl_scene_container_2d_t container, uint32_t id);

typedef struct /* TODO: Move this somewhere else */
{
   uint32_t id;
   float position[3];
} kl_raycast_hit_t;

/**
 * Raycast.
 *
 * Cast a ray into the scene container and return the hit closest to the start
 * of the ray.
 *
 * @param  container   Scene container into which the ray should be cast.
 * @param  from_xy     Start point of ray.
 * @param  to_xy       End point of ray.
 * @param  typemask    Typemask against which valid hits should be tested.
 * @param  out_hit     kl_raycast_hit_t structure to hold result.
 *
 * @return -1 if input ray is invalid.
 *         0 if no intersections found.
 *         Number of intersections otherwise.
 */
extern KL_API int kl_raycast_scene_container_2d(kl_scene_container_2d_t container, float* KL_RESTRICT from_xy,
   float* KL_RESTRICT to_xy, uint32_t typemask, kl_raycast_hit_t* KL_RESTRICT out_hit);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
