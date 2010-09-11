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
 
#ifndef _KL_SCRIPT_H_
#define _KL_SCRIPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"

#define kl_script_event_t_name_length (sizeof(int) * 4)
typedef struct
{
   //! The event name.
   char name[kl_script_event_t_name_length];
   
   //! An opaque pointer to a context object, or NULL/nil.
   void* context;
   
   int a, b, c; // struct length should be 64 bytes
} kl_script_event_t;

typedef struct _kl_script_context* kl_script_context_t;

#define KL_DEFAULT_SCRIPT_CONTEXT NULL

//! Allocate and initialize a script-task processor.
//!
//! @note This function assumes that kl_script_queue has been initialized.
//!
//! @param context This holds the allocated script context.
//! @param event_queue_size The memory size of the event queue for this script context.
//! @return KL_SUCCESS if initialization was successful.
//!         KL_ERROR if initialization failed.
extern KL_API int kl_script_init(kl_script_context_t* context, KL_BOOL threaded, size_t event_queue_size);

//! Immediately evaluate a script event.
extern KL_API int kl_script_evaluate();

//! Run a script file
extern KL_API int kl_script_run(kl_script_context_t context, const char* file_name, int argc, const char** argv);

//! Destroy a script-task processor.
//!
//! Destroys a script-task processor and frees the associated memory.
//!
//! @note This function does not free, or modify kl_script_queue.
//!
//! @param context This script-context will be deallocated.
extern KL_API void kl_script_destroy(kl_script_context_t* context);

//! Check if the script-context is threaded.
//!
//! @param context The script-context to query.
//! @return KL_TRUE if the script-context is threaded.
//!         KL_FALSE if the script-context is not threaded.
extern KL_API KL_BOOL kl_script_is_threaded(kl_script_context_t context);

// Event-queue manipulation
extern KL_API int kl_script_event_enqueue(kl_script_context_t context, const kl_script_event_t* event);
extern KL_API int kl_script_event_dequeue(kl_script_context_t context, kl_script_event_t* event);
extern KL_API int kl_script_event_pump(kl_script_context_t context);

#ifdef __cplusplus
}
#endif

#endif