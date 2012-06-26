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
 
#ifndef _KL_RARRAY_H_
#define _KL_RARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>
#include <stdint.h>

typedef struct {
   void* elements;
   uint32_t num_elements;
   size_t element_sz;
   uint32_t max_idx;
} kl_rarray_t;

extern KL_API int kl_rarray_init(kl_rarray_t* array, size_t element_sz, uint32_t num_elements);

extern KL_API void kl_rarray_destroy(kl_rarray_t* array);

extern KL_API void kl_rarray_resize(kl_rarray_t* array, uint32_t num_elements);

extern KL_API void kl_rarray_append(kl_rarray_t* array, const void* element);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
