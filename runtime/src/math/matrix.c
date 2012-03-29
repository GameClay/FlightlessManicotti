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

void kl_matrix_mul_matrix(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c)
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
void kl_matrix_mul_matrix_c(const float* KL_RESTRICT pA, const float* KL_RESTRICT pB, float* KL_RESTRICT pC)
{
   pC[0]  = pA[0]*pB[0] + pA[1]*pB[4] + pA[2]*pB[8]  + pA[3]*pB[12];
   pC[1]  = pA[0]*pB[1] + pA[1]*pB[5] + pA[2]*pB[9]  + pA[3]*pB[13];
   pC[2]  = pA[0]*pB[2] + pA[1]*pB[6] + pA[2]*pB[10] + pA[3]*pB[14];
   pC[3]  = pA[0]*pB[3] + pA[1]*pB[7] + pA[2]*pB[11] + pA[3]*pB[15];

   pC[4]  = pA[4]*pB[0] + pA[5]*pB[4] + pA[6]*pB[8]  + pA[7]*pB[12];
   pC[5]  = pA[4]*pB[1] + pA[5]*pB[5] + pA[6]*pB[9]  + pA[7]*pB[13];
   pC[6]  = pA[4]*pB[2] + pA[5]*pB[6] + pA[6]*pB[10] + pA[7]*pB[14];
   pC[7]  = pA[4]*pB[3] + pA[5]*pB[7] + pA[6]*pB[11] + pA[7]*pB[15];

   pC[8]  = pA[8]*pB[0] + pA[9]*pB[4] + pA[10]*pB[8]  + pA[11]*pB[12];
   pC[9]  = pA[8]*pB[1] + pA[9]*pB[5] + pA[10]*pB[9]  + pA[11]*pB[13];
   pC[10] = pA[8]*pB[2] + pA[9]*pB[6] + pA[10]*pB[10] + pA[11]*pB[14];
   pC[11] = pA[8]*pB[3] + pA[9]*pB[7] + pA[10]*pB[11] + pA[11]*pB[15];

   pC[12] = pA[12]*pB[0] + pA[13]*pB[4] + pA[14]*pB[8] + pA[15]*pB[12];
   pC[13] = pA[12]*pB[1] + pA[13]*pB[5] + pA[14]*pB[9] + pA[15]*pB[13];
   pC[14] = pA[12]*pB[2] + pA[13]*pB[6] + pA[14]*pB[10] + pA[15]*pB[14];
   pC[15] = pA[12]*pB[3] + pA[13]*pB[7] + pA[14]*pB[11] + pA[15]*pB[15];
}

