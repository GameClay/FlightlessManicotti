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

#include <FlightlessManicotti/math/vector.h>
#include <pmmintrin.h>

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

float kl_vector_dot_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b)
{
   return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]);
}

void kl_vector_math_self_test()
{
   int i;
   kl_vector4_t v1, v2;

#define RFl ((float)random() / RAND_MAX)
   for(i = 0; i < 4; i++) v1.xyzw[i] = RFl;
   for(i = 0; i < 4; i++) v2.xyzw[i] = RFl;
#undef RFl

   KL_ASSERT(fabs(kl_vector_dot(v1.xyzw, v2.xyzw) - kl_vector_dot_c(v1.xyzw, v2.xyzw)) < KL_EPSILON_F,
      "Mismatch in vector dot product");
}

kl_math_f_ab_restrict_fn kl_vector_dot = kl_vector_dot_sse3;
