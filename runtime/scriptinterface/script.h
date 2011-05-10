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
 
#ifndef _KL_SCRIPT_H_
#define _KL_SCRIPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include "scriptinterface/scriptevents.h"

//! Opaque pointer to a script-event fence.
//!
//! @see kl_script_event_endframe
typedef struct
{
   KL_BOOL processed;
} kl_script_event_fence_t;

//! Opaque pointer to a script-context.
//! 
//! @see kl_script_init
typedef struct _kl_script_context* kl_script_context_t;

//! The script-context which is allocated by kl_initialize()
//!
//! You may allocate, and use, other script contexts, as well.
//!
//! @see kl_script_init
#define KL_DEFAULT_SCRIPT_CONTEXT NULL

//! Allocate and initialize a script-context.
//!
//! This function may be called to allocate additional script-context. 
//! kl_initialize() creates a script context which should be used for
//! most cases.
//! 
//! A script-context may run threaded, or non-threaded. If the context is
//! allocated without threading, kl_script_event_pump() must be called
//! to process a frame of script.
//!
//! @see kl_script_is_threaded
//!
//! @param context This holds the allocated script context.
//! @param threaded Should this script-context run in a thread.
//! @param event_queue_max The maximum number of events that can fit in the event queue for this script context.
//! @return KL_SUCCESS if initialization was successful.
//!         KL_ERROR if initialization failed.
extern KL_API int kl_script_init(kl_script_context_t* context, KL_BOOL threaded, size_t event_queue_max);

//! Immediately evaluate a script event.
extern KL_API int kl_script_evaluate();

//! Run a script file
extern KL_API int kl_script_run(kl_script_context_t context, const char* file_name, int argc, const char** argv);

//! Destroy a script-context.
//!
//! If this script-context is threaded, this function will call
//! join on the associated thread, and then destroy the context.
//!
//! @param context This script-context will be deallocated.
extern KL_API void kl_script_destroy(kl_script_context_t* context);

//! Check if the script-context is threaded.
//!
//! @param context The script-context to query.
//! @return KL_TRUE if the script-context is threaded.
//!         KL_FALSE if the script-context is not threaded.
extern KL_API KL_BOOL kl_script_is_threaded(kl_script_context_t context);

//! Enqueue a script event.
//!
//! @param context The script-context on which to enqueue an event.
//! @param event The event to enqueue.
//! @return KL_SUCCESS if successful.
//!         KL_ERROR if unsuccessful.
extern KL_API int kl_script_event_enqueue(kl_script_context_t context, const kl_script_event_t* event);

//! Dequeue a script event.
//!
//! @param context The script-context from which to dequeue an event.
//! @param event Location to store dequeued event.
//! @return KL_SUCCESS if successful.
//!         KL_ERROR if unsuccessful.
extern KL_API int kl_script_event_dequeue(kl_script_context_t context, kl_script_event_t* event);

//! Pump the script-event queue.
//!
//! Process a frame worth of events.
//!
//! @param context The script-context to pump events through.
//! @return KL_SUCCESS if the frame processed successfully.
//!         KL_ERROR if errors occured during frame processing.
extern KL_API int kl_script_event_pump(kl_script_context_t context);

//! End a frame of script-events.
//!
//! Script events are processed in frames. Calling this function on a 
//! script-context signifies that there are no more events for this 
//! frame. A fence parameter may be specified in order to allow the
//! caller to wait on the fence.
//!
//! @param context The script-context on which to push an end-of-frame event.
//! @param fence Optional event-fence on which the caller can wait, or NULL if 
//!               this functionality is not necesassary.
//! @return KL_SUCCESS if successful.
//!         KL_ERROR if unsuccessful.
extern KL_API int kl_script_event_endframe(kl_script_context_t context, kl_script_event_fence_t* fence);


extern KL_API int kl_script_event_fence_wait(const kl_script_event_fence_t* fence);

extern KL_API int kl_script_event_fence_notify(kl_script_event_fence_t* fence);

#ifdef __cplusplus
}
#endif

#endif