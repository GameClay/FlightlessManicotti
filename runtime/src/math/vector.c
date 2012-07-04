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

#include <FlightlessManicotti/math/vector.h>
#include <FlightlessManicotti/core/timer.h>
#include <pmmintrin.h>
#include "math/vector_internal.h"

void kl_vector_math_self_test(void);

#if 3 <= __SSE__ || defined(__SSE3__)
float kl_vector_dot_sse3(const float* KL_RESTRICT a, const float* KL_RESTRICT b)
{
   kl_float32x4_t v0, v1, vr;
   float ret;

   v0 = kl_load_float32x4(a);
   v1 = kl_load_float32x4(b);

   vr = _mm_mul_ps(v0, v1);  /* vr = {x0 * x1, y0 * y1, z0 * z1, w0 * w1} */
   vr = _mm_hadd_ps(vr, vr); /* vr = {x0 * x1 + y0 * y1, z0 * z1 + w0 * w1,
                                      x0 * x1 + y0 * y1, z0 * z1 + w0 * w1} */
   vr = _mm_hadd_ps(vr, vr); /* vr = {x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,
                                      x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,
                                      x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,
                                      x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1} */

   _mm_store_ss(&ret, vr);
   return ret;
}
#endif

float kl_vector_dot_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b)
{
   return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]);
}

void kl_vector_cross_sse(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c)
{
   kl_float32x4_t v0, v1, vr;

   v0 = kl_load_float32x4(a);
   v1 = kl_load_float32x4(b);
   vr = _mm_sub_ps(
      _mm_mul_ps(_mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2))),
      _mm_mul_ps(_mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1)))
   );

   kl_store_float32x4(c, vr);
}

void kl_vector_cross_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c)
{
   c[0] = a[1] * b[2] - a[2] * b[1];
   c[1] = a[2] * b[0] - a[0] * b[2];
   c[2] = a[0] * b[1] - a[1] * b[0];
}

#define NUM_TEST_RUNS 50000
void kl_vector_math_self_test(void)
{
   int i, j;
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   float c_dot_ms, sse_dot_ms;
   kl_vector4_t v1[NUM_TEST_RUNS], v2[NUM_TEST_RUNS], v3[NUM_TEST_RUNS], v4[NUM_TEST_RUNS];

#define RFl ((float)random() / RAND_MAX)
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 4; i++) v1[j].v[i] = RFl;
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 4; i++) v2[j].v[i] = RFl;
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 4; i++) v3[j].v[i] = RFl;
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 4; i++) v4[j].v[i] = RFl;
#undef RFl

   KL_ASSERT(fabs(kl_vector_dot(v1[0].v, v2[0].v) - kl_vector_dot_c(v1[0].v, v2[0].v)) < KL_EPSILON_F,
      "Mismatch in vector dot product");

   /* Dot product benchmarking */
#if 3 <= __SSE__ || defined(__SSE3__)
   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < NUM_TEST_RUNS; i++)
      kl_vector_dot_sse3(v1[i].v, v2[i].v);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   sse_dot_ms = (float)time_ns * 1e-6f;
#endif

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < NUM_TEST_RUNS; i++)
      kl_vector_dot_c(v3[i].v, v4[i].v);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   c_dot_ms = (float)time_ns * 1e-6f;

   kl_log_debug("Vector self tests passed.\n\tDot product %d iterations: SSE %fms, C %fms",
      NUM_TEST_RUNS, sse_dot_ms, c_dot_ms);
}

#if 3 <= __SSE__ || defined(__SSE3__)
kl_math_f_ab_restrict_fn kl_vector_dot = kl_vector_dot_sse3;
#else
kl_math_f_ab_restrict_fn kl_vector_dot = kl_vector_dot_c;
#endif
kl_math_abc_restrict_fn kl_vector_cross = kl_vector_cross_sse;
