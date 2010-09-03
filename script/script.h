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

#include "core/queue.h"

typedef struct
{
   int event_id;
   int sender_id;
   int payload_size;
   void* payload;
} gc_script_event_TEMP;

typedef struct _gc_script_context* gc_script_context;

/// Initialize the script-task processor.
///
/// @note This function assumes that gc_script_queue has been initialized.
///
/// @return GC_SUCCESS if initialization was successful.
///         GC_ERROR if initialization failed.
int gc_script_init(gc_script_context* context);

/// Immediately evaluate a script event.
///
/// 
int gc_script_evaluate();

/// Destroy the script-task processor.
///
/// @note This function does not free, or modify gc_script_queue.
void gc_script_destroy(gc_script_context* context);

/// Commnication queue for the script-task processor.
///
/// All communication to and from the scripting engine takes place on this
/// queue.
///
/// @see gc_script_event_TEMP
///
/// @note It is possible to immediately execute a script event, but most
///       tasks should be processed using this queue. 
extern gc_queue gc_script_queue;

#ifdef __cplusplus
}
#endif

#endif