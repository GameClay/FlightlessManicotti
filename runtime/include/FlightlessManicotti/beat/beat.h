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
 
#ifndef _KL_BEAT_H_
#define _KL_BEAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/process/process.h>

typedef struct {
   uint32_t pid;
   kl_process_manager_t process_manager;

   float beat_interp;
   float measure_interp;

   uint64_t beat_frequency_ns;
   uint64_t last_beat_time_ns;
   uint64_t last_measure_time_ns;
   uint64_t measure_count;
   uint64_t beat_count;
} _kl_beat_manager, *kl_beat_manager_t;

#define KL_DEFAULT_BEAT_MANAGER NULL

/**
 * Allocate a beat manager.
 *
 * @param manager    Location to store allocated beat manager.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_beat_manager_alloc(kl_beat_manager_t* manager);

/**
 * Free a beat manager.
 *
 * @param manager    Beat manager to free.
 */
extern KL_API void kl_beat_manager_free(kl_beat_manager_t* manager);

/**
 * Get the default beat manager.
 *
 * @return Default beat manager.
 */
extern KL_API kl_beat_manager_t kl_beat_manager_default();

#ifdef __cplusplus
}
#endif

#endif
