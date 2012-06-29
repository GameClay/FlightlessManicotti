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

#ifndef _KL_CONSTANT_BUFFER_H_
#define _KL_CONSTANT_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KL_SHADER_CONSTANT_TYPE_NONE   0
#define KL_SHADER_CONSTANT_TYPE_MATRIX 1
#define KL_SHADER_CONSTANT_TYPE_FLOAT  2
#define KL_SHADER_CONSTANT_TYPE_INT    3
#define KL_SHADER_CONSTANT_TYPE_TEX    4

typedef struct _kl_new_shader_constant_t* kl_new_shader_constant_ptr;

typedef struct _kl_new_shader_constant_t {
   union {
      void* as_ptr;
      float* as_float_ptr;
      int* as_int_ptr;
      float as_float;
      int as_int;
   } constant;
   uint32_t constant_sz;
   uint16_t constant_type;
   uint16_t dealloc_constant;
   uint32_t constant_num;
   int32_t constant_idx;
} kl_new_shader_constant_t;

typedef struct kl_shader_constant_buffer_t {
   size_t num_constants;
   kl_new_shader_constant_t constant[];
} kl_shader_constant_buffer_t;

#ifdef __cplusplus
}
#endif

#endif
