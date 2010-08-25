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
 
#ifndef GC_THREADS_H_
#define GC_THREADS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ccompat.h"

#define thrd_success 0
#define thrd_nomem 1
#define thrd_error 2

typedef int (*thrd_start_t)(void* arg);

#if defined(__linux) || defined(__APPLE__)
#include <pthread.h>
#include <errno.h>
#include <sched.h>

typedef pthread_t thrd_t;

typedef void* (*_gcstart_routine_compat)(void*);

typedef struct
{
   time_t sec; 
   long nsec;
} xtime;

inline int thrd_create(thrd_t* thr, thrd_start_t func, void* arg)
{
   switch(pthread_create(thr, NULL, (_gcstart_routine_compat)func, arg))
   {
      case 0:      return thrd_success;
      case EAGAIN: return thrd_nomem;
      default:     return thrd_error;
   }
}

inline thrd_t thrd_current(void)
{
   return pthread_self();
}

inline int thrd_detach(thrd_t thr)
{
   switch(pthread_detach(thr))
   {
      case 0:  return thrd_success;
      default: return thrd_error;
   }
}

inline int thrd_equal(thrd_t thr0, thrd_t thr1)
{
   return pthread_equal(thr0, thr1);
}

inline void thrd_exit(int res)
{
   pthread_exit((void*)res);
}

inline int thrd_join(thrd_t thr, int* res)
{
   switch(pthread_join(thr, (void**)&res))
   {
      case 0:  return thrd_success;
      default: return thrd_error;
   }
}

inline void thrd_yield(void)
{
   sched_yield();
}

void thrd_sleep(const xtime* xt);

#define TIME_UTC 1
int xtime_get(xtime* xt, int base);

#endif // defined(__linux) || defined(__APPLE__)

#ifdef __cplusplus
}
#endif

#endif