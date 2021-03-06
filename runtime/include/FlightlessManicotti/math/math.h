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

#ifndef _KL_MATH_H_
#define _KL_MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/core/simd.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

#define KL_PI_F 3.14159265f
#define KL_2PI_F 6.28318531f
#define KL_EPSILON_F 0.00001f

#define KL_INT16_MAX_F 32767.0f

#define kl_fsel(a, x, y) (a >= 0.0f ? x : y)
#define kl_sin sinf
#define kl_cos cosf

#define kl_max(x, y) kl_fsel(y - x, y, x)
#define kl_min(x, y) kl_fsel(y - x, x, y)

#define kl_lerp(a, b, t) (a + t * (b - a))

#define kl_is_zero(f) kl_fsel(fabs(f) - KL_EPSILON_F, 1, 0)

extern KL_API float kl_nextpow2(float x);

typedef void (*kl_math_abc_restrict_fn)(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c);
typedef void (*kl_math_abcn_restrict_fn)(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c, uint32_t n);
typedef float (*kl_math_f_ab_restrict_fn)(const float* KL_RESTRICT a, const float* KL_RESTRICT b);

extern KL_API float kl_nextpow2(float x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
