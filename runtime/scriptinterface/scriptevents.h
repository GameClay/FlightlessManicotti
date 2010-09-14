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
 
#ifndef _KL_SCRIPT_EVENTS_H_
#define _KL_SCRIPT_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"
#include <xmmintrin.h> // TODO: This is not portable, neither is usage of '__m128'. Must typedef or something.
#include <stdint.h>

//! Script-event.
//!
//! 
typedef union
{
   uint64_t _dont_use64,   //!< Don't use this value.
            context64;     //!< Context as a 64-bit value.
            
   uint32_t id,            //!< Event id.
            arg,           //!< Event-defined argument.
            context,       //!< Context as a 32-bit value.
            _dont_use32;   //!< Don't use this value.
            
   //! Representation of the script-event as an __m128.
   __m128i as_m128i;
} kl_script_event_t;

extern KL_API uint32_t kl_register_script_event(const char* name);

#ifdef __cplusplus
}
#endif

#endif
