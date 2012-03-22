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

#ifndef _KL_SIMD_H_
#define _KL_SIMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KL_ARCH_X86) || defined(KL_ARCH_X64)

#include <emmintrin.h>

typedef __m128i kl_int32x4_t;
typedef __m128 kl_float32x4_t;

#define kl_load_int32x4(const_kl_int32x4_t_ptr) _mm_load_si128(const_kl_int32x4_t_ptr)
#define kl_store_int32x4(dest_int32_t_ptr, src_kl_int32x4_t) _mm_store_si128(dest_int32_t_ptr, src_kl_int32x4_t)

#define kl_load_float32x4(const_kl_float32x4_t) _mm_load_ps(const_kl_float32x4_t)
#define kl_store_float32x4(dest_float32_ptr, src_kl_float32x4_t) _mm_store_ps(dest_float32_ptr, src_kl_float32x4_t)

#elif defined(KL_ARCH_NEON)

#include <arm_neon.h>

typedef int32x4_t kl_int32x4_t;

#define kl_load_int32x4(const_kl_int32x4_t_ptr) vld1q_s32((int32_t*)const_kl_int32x4_t_ptr)
#define kl_store_int32x4(dest_int32_t_ptr, src_kl_int32x4_t) vst1q_s32((int32_t*)dest_int32_t_ptr, src_kl_int32x4_t)

#else

#include <stdint.h>
#include <FlightlessManicotti/core/memory.h>

KL_ALIGNED_STRUCT_PRE(16)
{
   int32_t a;
   int32_t b;
   int32_t c;
   int32_t d;
}KL_ALIGNED_STRUCT_POST(kl_int32x4_t, 16);

#define kl_load_int32x4(const_kl_int32x4_t_ptr) (*(const_kl_int32x4_t_ptr))
#define kl_store_int32x4(dest_int32_t_ptr, src_kl_int32x4_t) kl_microrcpy(dest_int32_t_ptr, &(src_kl_int32x4_t), sizeof(kl_int32x4_t))

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _KL_SIMD_H_ */
