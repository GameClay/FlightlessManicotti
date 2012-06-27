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

#ifndef _KL_MATRIX_H_
#define _KL_MATRIX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/math/math.h>
#include <FlightlessManicotti/math/vector.h>

/* Row Major
 * x0 x1 x2 x3
 * y0 y1 y2 y3
 * z0 z1 z2 z3
 * w0 w1 w2 w3
 */

KL_ALIGNED_STRUCT_PRE(16) {
   float m[16];
} KL_ALIGNED_STRUCT_POST(kl_matrix_t, 16);

/**
 * Matrix * Matrix
 *
 * @param a Matrix A.
 * @param b Matrix B.
 * @param c Output matrix.
 *
 * @relates kl_matrix_t
 */
extern kl_math_abc_restrict_fn kl_matrix_mul_matrix;

/**
 * Matrix * Vector
 *
 * @param a Matrix.
 * @param b Vector.
 * @param c Output vector.
 *
 * @relates kl_matrix_t
 */
extern kl_math_abc_restrict_fn kl_matrix_mul_vector;

/**
 * Matrix * Vector batch
 *
 * @param a Matrix.
 * @param b Vector array.
 * @param c Output vector array.
 * @param n Number of vector elements in b and c.
 *
 * @relates kl_matrix_t
 */
extern kl_math_abcn_restrict_fn kl_matrix_mul_vector_batch;

/**
 * Create a perspective projection matrix.
 *
 * @param m       Matrix.
 * @param left    Left.
 * @param right   Right.
 * @param near    Near.
 * @param far     Far.
 *
 * @relates kl_matrix_t
 */
extern KL_API void kl_matrix_perspective(float* m, float left, float right, float bottom, float top, float near, float far);

/**
 * Create an orthographic projection matrix.
 *
 * @param m       Matrix.
 * @param left    Left.
 * @param right   Right.
 * @param near    Near.
 * @param far     Far.
 *
 * @relates kl_matrix_t
 */
extern KL_API void kl_matrix_ortho(float* m, float left, float right, float bottom, float top, float near, float far);

/**
 * Initialize a matrix to the identity matrix.
 *
 * @relates kl_matrix_t
 */
extern KL_API void kl_matrix_identity(float* m);

/**
 * Create a scaling matrix.
 *
 * @param m       Matrix.
 * @param x       X scale.
 * @param y       Y scale.
 * @param z       Z scale.
 *
 * @relates kl_matrix_t
 */
extern KL_API void kl_matrix_scale(float* m, float x, float y, float z);

/**
 * Dump a matrix to the console.
 *
 * @param m    Matrix.
 */
extern KL_API void kl_matrix_dump(float* m);

/**
 * Test SIMD vs C implementations of matrix math functions.
 *
 * Asserts if a failure occurs.
 */
extern KL_API void kl_matrix_math_self_test(void);

/**
 * Constant identity matrix.
 */
extern const kl_matrix_t KL_MATRIX_IDENTITY;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
