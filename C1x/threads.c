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

#include "C1x/threads.h"

#if defined(__linux) || defined(__APPLE__)
#include <unistd.h>
#include <sys/time.h>

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