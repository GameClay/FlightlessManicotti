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

#ifndef _KL_SCENE_CONTAINER_3D_H_
#define _KL_SCENE_CONTAINER_3D_H_

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

   float* transform;                      /**< 4x4 transforms for entries in the scene. Indexed by id * 16. */

   float* radius;                         /**< Circular bounds radius for entires in the scene. Indexed by id. */
}*kl_scene_container_3d_t, _kl_scene_container_3d;

enum
{
   SceneType_Reserved         = 1 << 0   /**< This entry has been reserved. */
};

/**
 * Allocate a 3d scene container.
 *
 * @param container       Location to store allocated scene container.
 * @param process_manager Proccess manager on which to sign up to advance-time/process-tick.
 * @param max_entries     Maximum number of objects which can be managed by this container.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_alloc_scene_container_3d(kl_scene_container_3d_t* container, 
   kl_process_manager_t process_manager, uint32_t max_entries);

/**
 * Free a 3d scene container.
 *
 * @param container    Scene container to free.
 */
extern KL_API void kl_free_scene_container_3d(kl_scene_container_3d_t* container);

/**
 * Reserve an index in the scene container.
 *
 * @param container    Scene container from which to reserve an id.
 * @param out_id       Storage location for id, if reservation is successful.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_reserve_scene_container_3d_id(kl_scene_container_3d_t container, uint32_t* out_id);

/**
 * Free a previously reserved scene id.
 *
 * @param container    Scene container on which to free the id.
 * @param id           Id to free.
 */
extern KL_API void kl_free_scene_container_3d_id(kl_scene_container_3d_t container, uint32_t id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
