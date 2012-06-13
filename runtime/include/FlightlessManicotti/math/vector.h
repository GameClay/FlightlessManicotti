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

#ifndef _KL_VECTOR_H_
#define _KL_VECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/math/math.h>

typedef KL_ALIGNPRE(16) union {
   float v[4];
   kl_float32x4_t vec;
} kl_vector4_t KL_ALIGNPOST(16);

extern kl_math_f_ab_restrict_fn kl_vector_dot;

extern kl_math_abc_restrict_fn kl_vector_cross;

extern KL_API void kl_vector_math_self_test();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif