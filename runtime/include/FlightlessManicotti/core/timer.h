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

#ifndef _KL_TIMER_H_
#define _KL_TIMER_H_

// TODO: Proper platform stuff
#include <mach/mach_time.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>

#define kl_absolute_time_t uint64_t

//! Get the current time from the highest-resolution timer available on the platform.
//!
//! @param time_ptr  Pointer to a high-resolution timer structure.
#define kl_high_resolution_timer_query(time_ptr) (*time_ptr = mach_absolute_time())

//! Convert absolute-time to nanoseconds.
//!
//! @param time   Pointer to absolute-time to convert to nanoseconds.
//! @param out_ns The resulting time in nanoseconds.
extern KL_API void kl_absolute_time_to_ns(const kl_absolute_time_t* time, uint64_t* out_ns);

//! Convert nanoseconds to absolute-time.
//! @param  ns                Pointer to nanoseconds to convert to absolute-time.
//! @param  out_absolute_time The resulting absolute time.
extern KL_API void kl_ns_to_absolute_time(const uint64_t* ns, kl_absolute_time_t* out_absolute_time);

#ifdef __cplusplus
}
#endif

#endif
