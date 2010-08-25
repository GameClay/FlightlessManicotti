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

#define thrd_success int(0)
#define thrd_nomem int(1)
#define thrd_error int(2)

typedef void* (*thrd_start_t)(void* arg);

typedef struct
{
   
} thrd_t;

typedef struct
{
   
} xtime;

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg); 
thrd_t thrd_current(void); 
int thrd_detach(thrd_t thr); 
int thrd_equal(thrd_t thr0, thrd_t thr1); 
void thrd_exit(int res); 
int thrd_join(thrd_t thr, int *res); 
void thrd_sleep(const xtime *xt); 
void thrd_yield(void);

#ifdef __cplusplus
}
#endif

#endif