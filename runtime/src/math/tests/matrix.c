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

#include <FlightlessManicotti/math/matrix.h>
#include <FlightlessManicotti/core/timer.h>

float kl_matrix_mul_matrix_sse_timing(uint32_t num)
{
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   int i, j;

   kl_matrix_t* a = kl_heap_alloc(sizeof(kl_matrix_t) * num);
   kl_matrix_t* b = kl_heap_alloc(sizeof(kl_matrix_t) * num);
   kl_matrix_t c;

#define RFl ((float)random() / RAND_MAX)
   for(j = 0; j < num; j++) for(i = 0; i < 16; i++) a[j].m[i] = RFl;
   for(j = 0; j < num; j++) for(i = 0; i < 16; i++) b[j].m[i] = RFl;
#undef RFl

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < num; i++)
      kl_matrix_mul_matrix_sse(a[i].m, b[i].m, c.m);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);

   kl_heap_free(a);
   kl_heap_free(b);

   return (float)time_ns * 1e-6;
}

float kl_matrix_mul_matrix_c_timing(uint32_t num)
{
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   int i, j;

   kl_matrix_t* a = kl_heap_alloc(sizeof(kl_matrix_t) * num);
   kl_matrix_t* b = kl_heap_alloc(sizeof(kl_matrix_t) * num);
   kl_matrix_t c;

#define RFl ((float)random() / RAND_MAX)
   for(j = 0; j < num; j++) for(i = 0; i < 16; i++) a[j].m[i] = RFl;
   for(j = 0; j < num; j++) for(i = 0; i < 16; i++) b[j].m[i] = RFl;
#undef RFl

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < num; i++)
      kl_matrix_mul_matrix_c(a[i].m, b[i].m, c.m);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);

   kl_heap_free(a);
   kl_heap_free(b);

   return (float)time_ns * 1e-6;
}

float kl_matrix_mul_vector_sse3_timing(uint32_t num)
{
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   int i, j;

   kl_matrix_t* a = kl_heap_alloc(sizeof(kl_matrix_t) * num);
   kl_vector4_t* b = kl_heap_alloc(sizeof(kl_vector4_t) * num);
   kl_vector4_t* c = kl_heap_alloc(sizeof(kl_vector4_t) * num);

#define RFl ((float)random() / RAND_MAX)
   for(j = 0; j < num; j++) for(i = 0; i < 16; i++) a[j].m[i] = RFl;
   for(j = 0; j < num; j++) for(i = 0; i < 4; i++) b[j].xyzw[i] = RFl;
#undef RFl

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < num; i++)
      kl_matrix_mul_vector_sse3(a[i].m, b[i].xyzw, c[i].xyzw);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);

   kl_heap_free(a);
   kl_heap_free(b);
   kl_heap_free(c);

   return (float)time_ns * 1e-6;
}

float kl_matrix_mul_vector_c_timing(uint32_t num)
{
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   int i, j;

   kl_matrix_t* a = kl_heap_alloc(sizeof(kl_matrix_t) * num);
   kl_vector4_t* b = kl_heap_alloc(sizeof(kl_vector4_t) * num);
   kl_vector4_t* c = kl_heap_alloc(sizeof(kl_vector4_t) * num);

#define RFl ((float)random() / RAND_MAX)
   for(j = 0; j < num; j++) for(i = 0; i < 16; i++) a[j].m[i] = RFl;
   for(j = 0; j < num; j++) for(i = 0; i < 4; i++) b[j].xyzw[i] = RFl;
#undef RFl

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < num; i++)
      kl_matrix_mul_vector_c(a[i].m, b[i].xyzw, c[i].xyzw);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);

   kl_heap_free(a);
   kl_heap_free(b);
   kl_heap_free(c);

   return (float)time_ns * 1e-6;
}

float kl_matrix_mul_vector_batch_sse3_timing(uint32_t num)
{
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   int i, j;

   kl_matrix_t a;
   kl_vector4_t* b = kl_heap_alloc(sizeof(kl_vector4_t) * num);
   kl_vector4_t* c = kl_heap_alloc(sizeof(kl_vector4_t) * num);

#define RFl ((float)random() / RAND_MAX)
   for(i = 0; i < 16; i++) a.m[i] = RFl;
   for(j = 0; j < num; j++) for(i = 0; i < 4; i++) b[j].xyzw[i] = RFl;
#undef RFl

   kl_high_resolution_timer_query(&start_time);
   kl_matrix_mul_vector_batch_sse3(a.m, (float*)b, (float*)c, num);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);

   kl_heap_free(b);
   kl_heap_free(c);

   return (float)time_ns * 1e-6;
}

float kl_matrix_mul_vector_batch_c_timing(uint32_t num)
{
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   int i, j;

   kl_matrix_t a;
   kl_vector4_t* b = kl_heap_alloc(sizeof(kl_vector4_t) * num);
   kl_vector4_t* c = kl_heap_alloc(sizeof(kl_vector4_t) * num);

#define RFl ((float)random() / RAND_MAX)
   for(i = 0; i < 16; i++) a.m[i] = RFl;
   for(j = 0; j < num; j++) for(i = 0; i < 4; i++) b[j].xyzw[i] = RFl;
#undef RFl

   kl_high_resolution_timer_query(&start_time);
   kl_matrix_mul_vector_batch_c(a.m, (float*)b, (float*)c, num);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);

   kl_heap_free(b);
   kl_heap_free(c);

   return (float)time_ns * 1e-6;
}
