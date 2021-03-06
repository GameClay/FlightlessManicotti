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
 
#ifndef _KL_SCRIPT_EVENTS_H_
#define _KL_SCRIPT_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/core/simd.h>
#include <stdint.h>

/**
 * Script-event.
 */
typedef union
{
   struct
   {
      uint32_t id;            /**< Event id. */

      union
      {
         uint32_t as_uint32;
         float as_float;
      } arg;                  /**< Event-defined argument. */
   
      union
      {
         uint64_t as_64;
         void* as_ptr;
      } context;
   } event;

   kl_int32x4_t as_int32x4; /**< Representation of the script-event as an 4-component integer vector. */
} kl_script_event_t;

#define KL_SCRIPT_CONTEXT_TYPE_ASSIGNER (UINT32_MAX)

typedef int (*kl_script_event_context_assigner_fn)(void* L, kl_script_event_t* event);

extern KL_API uint32_t kl_register_script_event(const char* name);

extern KL_API void kl_register_script_event_context_type(uint32_t id, uint32_t context_type,
   kl_script_event_context_assigner_fn assigner);

extern KL_API uint32_t kl_get_script_event_context_type(uint32_t id);

extern KL_API kl_script_event_context_assigner_fn kl_get_script_event_context_assigner(uint32_t id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
