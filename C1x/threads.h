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

#ifndef __STDC_NO_THREADS__

#ifdef __cplusplus
extern "C" {
#endif

#include "ccompat.h"

// thrd_* constants
#define thrd_success 0
#define thrd_nomem 1
#define thrd_error 2
#define thrd_busy 3
#define thrd_timeout 4

// mtx_* constants
#define mtx_recursive 1
#define mtx_plain 2
#define mtx_timed 4
#define mtx_try 8

typedef int (*thrd_start_t)(void* arg);

#if defined(__linux) || defined(__APPLE__)
#include <pthread.h>
#include <errno.h>
#include <sched.h>

typedef pthread_t thrd_t;
typedef pthread_mutex_t mtx_t;

typedef void* (*_gcstart_routine_compat)(void*);

typedef struct
{
   time_t sec; 
   long nsec;
} xtime;

// mtx_* functions
inline void mtx_destroy(mtx_t* mtx)
{
   pthread_mutex_destroy(mtx);
}

int mtx_init(mtx_t* mtx, int type); 

inline int mtx_lock(mtx_t* mtx)
{
   switch(pthread_mutex_lock(mtx))
   {
      case 0:       return thrd_success;
      //case EDEADLK: return thrd_busy;
      default:      return thrd_error;
   }
}

int mtx_timedlock(mtx_t* restrict mtx, const xtime* restrict xt)
{
   // Not implemented yet
   return thrd_error;
}

inline int mtx_trylock(mtx_t* mtx)
{
   switch(pthread_mutex_trylock(mtx))
   {
      case 0:     return thrd_success;
      case EBUSY: return thrd_busy;
      default:    return thrd_error;
   }
}

inline int mtx_unlock(mtx_t* mtx)
{
   switch(pthread_mutex_unlock(mtx))
   {
      case 0:  return thrd_success;
      default: return thrd_error;
   }
}

// thrd_* functions
inline int thrd_create(thrd_t* restrict thr, thrd_start_t func, void* restrict arg)
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

// xtime
#define TIME_UTC 1
int xtime_get(xtime* xt, int base);

#endif // defined(__linux) || defined(__APPLE__)

#ifdef __cplusplus
}
#endif

#endif //__STDC_NO_THREADS__

#endif