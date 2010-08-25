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

#include "threads.h"

#if defined(__linux) || defined(__APPLE__)
#include <unistd.h>
#include <sys/time.h>

int mtx_init(mtx_t* mtx, int type)
{
   pthread_mutexattr_t attr;
   if(pthread_mutexattr_init(&attr)) return thrd_error;
   
   int ret_val = thrd_error;
   
   // Mark as recursive or normal
   if((type & mtx_recursive) && pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) goto mtx_init_return;
   else if(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)) goto mtx_init_return;
   
   // Clear recursive flag bit, and process valid values
   type &= ~mtx_recursive;
   switch(type)
   {
      // Pthreads needs no such create flags
      case mtx_plain: break;
      case mtx_timed: break; 
      case mtx_try: break;
      
      default: goto mtx_init_return;
   }
   
   switch(pthread_mutex_init(mtx, &attr))
   {
      case 0: {ret_val = thrd_success; goto mtx_init_return;}
      default: goto mtx_init_return;
   }
   
mtx_init_return:
   pthread_mutexattr_destroy(&attr);
   
   return ret_val;
}

void thrd_sleep(const xtime* xt)
{
   usleep(xt->nsec);
}

int xtime_get(xtime* xt, int base)
{
   struct timezone tzp = {0, 0};
   struct timeval tp = {0, 0};
   
   switch(gettimeofday(&tp, &tzp))
   {
      case 0:
      {
         xt->sec = tp.tv_sec;
         xt->nsec = tp.tv_usec;
         return base;
      }
      default: return 0;
   }
}

#endif // defined(__linux) || defined(__APPLE__)