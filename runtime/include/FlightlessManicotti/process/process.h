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
 
#ifndef _KL_PROCESS_H_
#define _KL_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <stdint.h>
#include <FlightlessManicotti/scriptinterface/scriptevents.h>

/**
 * Opaque pointer to a process manager.
 */
typedef struct _kl_process_manager* kl_process_manager_t;

typedef void (*kl_process_tick_ptr)(void* context);
typedef void (*kl_process_advance_time_ptr)(float dt, void* context);

#define KL_DEFAULT_PROCESS_MANAGER NULL

/**
 * Allocate a process manager.
 *
 * @note   This should not be mistaken for the driver for game-objects.
 *         Every object in the game should not be an individual process, instead
 *         the process manager drives systems.
 *
 * @param mgr            Location to store newly allocated process manager.
 * @param num_processes  Maximum number of processes for the allocated manager.
 *
 * @return KL_SUCCESS if successful.
 *
 * @relates kl_process_manager_t
 */
extern KL_API int kl_alloc_process_manager(kl_process_manager_t* mgr, uint32_t num_processes);

/**
 * Free a process manager.
 *
 * @param mgr Process manager to free.
 *
 * @relates kl_process_manager_t
 */
extern KL_API void kl_free_process_manager(kl_process_manager_t* mgr);

/**
 * Reserve a process id, and insert advance-time, and process-tick functions.
 *
 * @param mgr              Process manager from which an id should be reserved.
 * @param tick_fn          Function pointer for advance-time calls, or NULL.
 * @param advance_time_fn  Function pointer for advance-time calls, or NULL.
 * @param context          Context which will be passed to tick_fn and advance_time_fn.
 *
 * @return The process id which has been reserved.
 *
 * @relates kl_process_manager_t
 */
extern KL_API uint32_t kl_reserve_process_id(kl_process_manager_t mgr,
   kl_process_tick_ptr tick_fn, kl_process_advance_time_ptr advance_time_fn, void* context);

/**
 * Release a process id.
 *
 * @param mgr  Process manager from which the id was allocated.
 * @param id   Process id to release.
 *
 * @relates kl_process_manager_t
 */
extern KL_API void kl_release_process_id(kl_process_manager_t mgr, uint32_t id);

/**
 * Tick the process list.
 *
 * This invokes the respective tick function on all registered processes.
 * A process receives a tick for every 32ms of time that passes. This means
 * That a process could get it's tick function called several times in 
 * rapid succession if the simulation needs to "catch up".
 *
 * @param mgr    The process manager to tick.
 *
 * @return KL_SUCCESS if successful.
 *
 * @relates kl_process_manager_t
 */
extern KL_API int kl_tick_process_list(const kl_process_manager_t mgr);

/**
 * Advance time on the process list.
 *
 * This invokes the respective advance time function on all registered processes.
 * A process will receive this call once per frame, every frame.
 *
 * @param mgr    The process object manager on which to advance time.
 * @param dt     The amount of time, in milliseconds, which has passed since the last time
 *               kl_advance_process_list was called on this manager.
 *
 * @return KL_SUCCESS if successful.
 *
 * @relates kl_process_manager_t
 */
extern KL_API int kl_advance_process_list(const kl_process_manager_t mgr, float dt);

/**
 * Advance time script event.
 */
extern kl_script_event_t g_advance_time_script_event;

/**
 * Tick script event.
 */
extern kl_script_event_t g_tick_script_event;

#ifdef __cplusplus
}
#endif

#endif
