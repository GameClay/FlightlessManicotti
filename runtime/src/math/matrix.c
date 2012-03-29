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

#include <FlightlessManicotti/math/matrix.h>
#include <FlightlessManicotti/core/timer.h>
#include <sanskrit/sklog.h>
#include <pmmintrin.h>

void kl_matrix_mul_matrix_sse(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c)
{
   kl_float32x4_t a0, a1, a2, a3, b0, b1, b2, b3, c0, c1, c2, c3;

   a0 = kl_load_float32x4(a +  0);
   a1 = kl_load_float32x4(a +  4);
   a2 = kl_load_float32x4(a +  8);
   a3 = kl_load_float32x4(a + 12);

   b0 = kl_load_float32x4(b +  0);
   b1 = kl_load_float32x4(b +  4);
   b2 = kl_load_float32x4(b +  8);
   b3 = kl_load_float32x4(b + 12);

   c0 = _mm_mul_ps(_mm_shuffle_ps(a0, a0, _MM_SHUFFLE(0, 0, 0, 0)), b0);
   c1 = _mm_mul_ps(_mm_shuffle_ps(a1, a1, _MM_SHUFFLE(0, 0, 0, 0)), b0);
   c2 = _mm_mul_ps(_mm_shuffle_ps(a2, a2, _MM_SHUFFLE(0, 0, 0, 0)), b0);
   c3 = _mm_mul_ps(_mm_shuffle_ps(a3, a3, _MM_SHUFFLE(0, 0, 0, 0)), b0);

   c0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a0, a0, _MM_SHUFFLE(1, 1, 1, 1)), b1), c0);
   c1 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a1, a1, _MM_SHUFFLE(1, 1, 1, 1)), b1), c1);
   c2 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a2, a2, _MM_SHUFFLE(1, 1, 1, 1)), b1), c2);
   c3 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a3, a3, _MM_SHUFFLE(1, 1, 1, 1)), b1), c3);

   c0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a0, a0, _MM_SHUFFLE(2, 2, 2, 2)), b2), c0);
   c1 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a1, a1, _MM_SHUFFLE(2, 2, 2, 2)), b2), c1);
   c2 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a2, a2, _MM_SHUFFLE(2, 2, 2, 2)), b2), c2);
   c3 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a3, a3, _MM_SHUFFLE(2, 2, 2, 2)), b2), c3);

   c0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a0, a0, _MM_SHUFFLE(3, 3, 3, 3)), b3), c0);
   c1 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a1, a1, _MM_SHUFFLE(3, 3, 3, 3)), b3), c1);
   c2 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a2, a2, _MM_SHUFFLE(3, 3, 3, 3)), b3), c2);
   c3 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(a3, a3, _MM_SHUFFLE(3, 3, 3, 3)), b3), c3);

   kl_store_float32x4(c +  0, c0);
   kl_store_float32x4(c +  4, c1);
   kl_store_float32x4(c +  8, c2);
   kl_store_float32x4(c + 12, c3);
}

/* For testing */
void kl_matrix_mul_matrix_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT mresult)
{
   mresult[0] = a[0]*b[0] + a[1]*b[4] + a[2]*b[8]  + a[3]*b[12];
   mresult[1] = a[0]*b[1] + a[1]*b[5] + a[2]*b[9]  + a[3]*b[13];
   mresult[2] = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
   mresult[3] = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];

   mresult[4] = a[4]*b[0] + a[5]*b[4] + a[6]*b[8]  + a[7]*b[12];
   mresult[5] = a[4]*b[1] + a[5]*b[5] + a[6]*b[9]  + a[7]*b[13];
   mresult[6] = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
   mresult[7] = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];

   mresult[8] = a[8]*b[0] + a[9]*b[4] + a[10]*b[8] + a[11]*b[12];
   mresult[9] = a[8]*b[1] + a[9]*b[5] + a[10]*b[9] + a[11]*b[13];
   mresult[10]= a[8]*b[2] + a[9]*b[6] + a[10]*b[10]+ a[11]*b[14];
   mresult[11]= a[8]*b[3] + a[9]*b[7] + a[10]*b[11]+ a[11]*b[15];

   mresult[12]= a[12]*b[0]+ a[13]*b[4]+ a[14]*b[8] + a[15]*b[12];
   mresult[13]= a[12]*b[1]+ a[13]*b[5]+ a[14]*b[9] + a[15]*b[13];
   mresult[14]= a[12]*b[2]+ a[13]*b[6]+ a[14]*b[10]+ a[15]*b[14];
   mresult[15]= a[12]*b[3]+ a[13]*b[7]+ a[14]*b[11]+ a[15]*b[15];
}

void kl_matrix_mul_vector_sse3(const float* KL_RESTRICT a, const float* KL_RESTRICT vec, float* KL_RESTRICT o)
{
   kl_float32x4_t v =  kl_load_float32x4(vec);
   kl_float32x4_t a0 = kl_load_float32x4(a +  0);
   kl_float32x4_t a1 = kl_load_float32x4(a +  4);
   kl_float32x4_t a2 = kl_load_float32x4(a +  8);
   kl_float32x4_t a3 = kl_load_float32x4(a + 12);

   kl_float32x4_t m0 = _mm_mul_ps(a0, v);
   kl_float32x4_t m1 = _mm_mul_ps(a1, v);
   kl_float32x4_t m2 = _mm_mul_ps(a2, v);
   kl_float32x4_t m3 = _mm_mul_ps(a3, v);

   kl_float32x4_t sum_01 = _mm_hadd_ps(m0, m1);
   kl_float32x4_t sum_23 = _mm_hadd_ps(m2, m3);
   kl_float32x4_t result = _mm_hadd_ps(sum_01, sum_23);

   kl_store_float32x4(o, result);
}

void kl_matrix_mul_vector_c(const float* KL_RESTRICT m, const float* KL_RESTRICT p, float* KL_RESTRICT presult)
{
   presult[0] = m[0]*p[0] + m[1]*p[1] + m[2]*p[2]  + m[3]*p[3];
   presult[1] = m[4]*p[0] + m[5]*p[1] + m[6]*p[2]  + m[7]*p[3];
   presult[2] = m[8]*p[0] + m[9]*p[1] + m[10]*p[2] + m[11]*p[3];
   presult[3] = m[12]*p[0]+ m[13]*p[1]+ m[14]*p[2] + m[15]*p[3];
}

void kl_matrix_mul_vector_batch_sse3(const float* KL_RESTRICT a, const float* KL_RESTRICT vec, float* KL_RESTRICT o, uint32_t n)
{
   uint32_t i;
   kl_float32x4_t a0 = kl_load_float32x4(a +  0);
   kl_float32x4_t a1 = kl_load_float32x4(a +  4);
   kl_float32x4_t a2 = kl_load_float32x4(a +  8);
   kl_float32x4_t a3 = kl_load_float32x4(a + 12);

   for(i = 0; i < n; i++)
   {
      kl_float32x4_t v =  kl_load_float32x4(vec + 4 * i);

      kl_float32x4_t m0 = _mm_mul_ps(a0, v);
      kl_float32x4_t m1 = _mm_mul_ps(a1, v);
      kl_float32x4_t m2 = _mm_mul_ps(a2, v);
      kl_float32x4_t m3 = _mm_mul_ps(a3, v);

      kl_float32x4_t sum_01 = _mm_hadd_ps(m0, m1);
      kl_float32x4_t sum_23 = _mm_hadd_ps(m2, m3);
      kl_float32x4_t result = _mm_hadd_ps(sum_01, sum_23);

      kl_store_float32x4(o + 4 * i, result);
   }
}

void kl_matrix_mul_vector_batch_c(const float* KL_RESTRICT m, const float* KL_RESTRICT vec, float* KL_RESTRICT o, uint32_t n)
{
   uint32_t i;
   for(i = 0; i < n; i++)
   {
      const float* p = vec + 4 * i;
      float* presult = o + 4 * i;
      presult[0] = m[0]*p[0] + m[1]*p[1] + m[2]*p[2]  + m[3]*p[3];
      presult[1] = m[4]*p[0] + m[5]*p[1] + m[6]*p[2]  + m[7]*p[3];
      presult[2] = m[8]*p[0] + m[9]*p[1] + m[10]*p[2] + m[11]*p[3];
      presult[3] = m[12]*p[0]+ m[13]*p[1]+ m[14]*p[2] + m[15]*p[3];
   }
}

#define NUM_TEST_RUNS 50000
void kl_matrix_math_self_test()
{
   int i, j;
   kl_absolute_time_t start_time, end_time, delta_time;
   uint64_t time_ns;
   float c_mul_ms, sse_mul_ms, c_xfm_ms, sse_xfm_ms, c_batch_xfm_ms, sse_batch_xfm_ms;

   kl_matrix_t a[NUM_TEST_RUNS];
   kl_matrix_t b[NUM_TEST_RUNS];
   kl_matrix_t c1;
   kl_matrix_t c2;
   kl_vector4_t v[NUM_TEST_RUNS];
   kl_vector4_t d1;
   kl_vector4_t d2;
   kl_vector4_t d[NUM_TEST_RUNS];
#define RFl ((float)random() / RAND_MAX)
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 16; i++) a[j].m[i] = RFl;
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 16; i++) b[j].m[i] = RFl;
   for(j = 0; j < NUM_TEST_RUNS; j++) for(i = 0; i < 4; i++) v[j].xyzw[i] = RFl;
#undef RFl

   kl_matrix_mul_matrix_sse(a[0].m, b[0].m, c1.m);
   kl_matrix_mul_matrix_c(a[0].m, b[0].m, c2.m);
   for(i = 0; i < 16; i++)
   {
      KL_ASSERT(fabs(c1.m[i] - c2.m[i]) < KL_EPSILON_F, "Mismatch in matrix-matrix multiply");
   }

   /* Matrix * Matrix benchmarking */
   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < NUM_TEST_RUNS; i++)
      kl_matrix_mul_matrix_sse(a[i].m, b[i].m, c2.m);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   sse_mul_ms = (float)time_ns * 1e-6;

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < NUM_TEST_RUNS; i++)
      kl_matrix_mul_matrix_c(a[i].m, b[i].m, c2.m);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   c_mul_ms = (float)time_ns * 1e-6;

   kl_matrix_mul_vector_sse3(a[0].m, v[0].xyzw, d1.xyzw);
   kl_matrix_mul_vector_c(a[0].m, v[0].xyzw, d2.xyzw);

   for(i = 0; i < 4; i++)
   {
      KL_ASSERT(fabs(d1.xyzw[i] - d2.xyzw[i]) < KL_EPSILON_F, "Mismatch in matrix-vector transform");
   }

   /* Matrix * Vector benchmarking */
   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < NUM_TEST_RUNS; i++)
      kl_matrix_mul_vector_sse3(a[i].m, v[i].xyzw, d1.xyzw);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   sse_xfm_ms = (float)time_ns * 1e-6;

   kl_high_resolution_timer_query(&start_time);
   for(i = 0; i < NUM_TEST_RUNS; i++)
      kl_matrix_mul_vector_c(a[i].m, v[i].xyzw, d1.xyzw);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   c_xfm_ms = (float)time_ns * 1e-6;

   /* Matrix * Vector batch benchmarking */
   kl_high_resolution_timer_query(&start_time);
   kl_matrix_mul_vector_batch_sse3(a[0].m, (float*)v, (float*)d, NUM_TEST_RUNS);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   sse_batch_xfm_ms = (float)time_ns * 1e-6;

   kl_high_resolution_timer_query(&start_time);
   kl_matrix_mul_vector_batch_c(a[0].m, (float*)v, (float*)d, NUM_TEST_RUNS);
   kl_high_resolution_timer_query(&end_time);
   delta_time = end_time - start_time;
   kl_absolute_time_to_ns(&delta_time, &time_ns);
   c_batch_xfm_ms = (float)time_ns * 1e-6;

   sklog("Matrix self tests passed. (%d iterations)\n" \
      "\tMatrix * Matrix: SSE %fms, C %fms\n" \
      "\tMatrix * Vector: SSE %fms, C %fms" \
      "\tMatrix * Vector Batch: SSE %fms, C %fms",
      NUM_TEST_RUNS, sse_mul_ms, c_mul_ms, sse_xfm_ms, c_xfm_ms, sse_batch_xfm_ms, c_batch_xfm_ms);
}

/* Assign function pointers dynamically later */
kl_math_abc_restrict_fn kl_matrix_mul_matrix = kl_matrix_mul_matrix_sse;
kl_math_abc_restrict_fn kl_matrix_mul_vector = kl_matrix_mul_vector_sse3;
kl_math_abcn_restrict_fn kl_matrix_mul_vector_batch = kl_matrix_mul_vector_batch_sse3;
