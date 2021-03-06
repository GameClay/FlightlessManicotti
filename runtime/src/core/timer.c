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
#include <FlightlessManicotti/core/timer.h>

#if defined(KL_OS_DARWIN) || defined(KL_OS_IOS)
static mach_timebase_info_data_t s_timebase_info;

void kl_absolute_time_to_ns(const kl_absolute_time_t* time, uint64_t* out_ns)
{
   if(s_timebase_info.denom == 0) mach_timebase_info(&s_timebase_info);
   
   *out_ns = ((*time) * s_timebase_info.numer) / s_timebase_info.denom;
}

void kl_ns_to_absolute_time(const uint64_t* ns, kl_absolute_time_t* out_absolute_time)
{
   if(s_timebase_info.denom == 0) mach_timebase_info(&s_timebase_info);
   
   *out_absolute_time = ((*ns) * s_timebase_info.denom) / s_timebase_info.numer;
}
#elif defined(KL_OS_WINDOWS)

#else
void kl_high_resolution_timer_query(kl_absolute_time_t* time_ptr)
{
   struct timespec clocktime;
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &clocktime);
   *time_ptr = clocktime.tv_sec * 1000000000 + clocktime.tv_nsec;
}
#endif
