/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2009-2010 GameClay LLC
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
 
#ifndef _KL_PROCESS_H_
#define _KL_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <stdint.h>

//! Opaque pointer to a process-object manager
typedef struct _kl_process_object_manager* kl_process_object_manager_t;

#define KL_DEFAULT_PROCESS_OBJECT_MANAGER NULL

extern KL_API int kl_alloc_process_object_manager(kl_process_object_manager_t* mgr, uint32_t num_objects);
extern KL_API void kl_free_process_object_manager(kl_process_object_manager_t* mgr);

extern KL_API uint32_t kl_reserve_process_object_id(kl_process_object_manager_t mgr);
extern KL_API void kl_release_process_object_id(kl_process_object_manager_t mgr, uint32_t id);

extern KL_API int kl_tick_process_object_list(const kl_process_object_manager_t mgr);
extern KL_API int kl_advance_process_object_list(const kl_process_object_manager_t mgr, float dt);

typedef void (*kl_process_object_tick_ptr)();
typedef void (*kl_process_object_advance_time_ptr)(float dt);

#ifdef __cplusplus
}
#endif

#endif