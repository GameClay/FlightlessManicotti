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
 
#ifndef GC_SCRIPT_H_
#define GC_SCRIPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <stdbool.h>

GC_API typedef struct
{
   int event_id;
   int sender_id;
   size_t payload_size;
   void* payload;
} gc_script_event_TEMP;

GC_API typedef struct _gc_script_context* gc_script_context;

/// Allocate and initialize a script-task processor.
///
/// @note This function assumes that gc_script_queue has been initialized.
///
/// @param context[out] This holds the allocated script context.
/// @param event_queue_size[in] The memory size of the event queue for this script context.
/// @return GC_SUCCESS if initialization was successful.
///         GC_ERROR if initialization failed.
GC_API int gc_script_init(gc_script_context* context, size_t event_queue_size);

/// Immediately evaluate a script event.
///
/// 
GC_API int gc_script_evaluate();

/// Run a script file
GC_API int gc_script_run(gc_script_context context, const char* file_name, bool threaded, int argc, const char** argv);

/// Destroy a script-task processor.
///
/// Destroys a script-task processor and frees the associated memory.
///
/// @note This function does not free, or modify gc_script_queue.
///
/// @param context[out] This script context will be deallocated.
GC_API void gc_script_destroy(gc_script_context* context);


// Event-queue manipulation
GC_API int gc_script_event_push(gc_script_context context, const gc_script_event_TEMP* event);
GC_API int gc_script_event_pop(gc_script_context context, gc_script_event_TEMP* event);

#ifdef __cplusplus
}
#endif

#endif