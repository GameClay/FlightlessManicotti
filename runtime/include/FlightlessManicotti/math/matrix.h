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

#ifndef _KL_MATRIX_H_
#define _KL_MATRIX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/math/math.h>
#include <FlightlessManicotti/core/simd.h>

/* x0 x1 x2 x3
 * y0 y1 y2 y3
 * z0 z1 z2 z3
 * w0 w1 w2 w3
 */

extern KL_API void kl_matrix_mul_matrix(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
