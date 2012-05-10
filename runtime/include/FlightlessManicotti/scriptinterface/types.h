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
 
#ifndef _KL_SCRIPTTYPES_H_
#define _KL_SCRIPTTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
   size_t len;
   float* array;
} kl_lua_float_array_t;

typedef struct {
   size_t len;
   uint16_t* array;
} kl_lua_uint16_array_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
