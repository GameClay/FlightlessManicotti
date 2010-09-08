/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2009-2010 GameClay LLC
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
 
#ifndef _KL_CCOMPAT_H_
#define _KL_CCOMPAT_H_

#ifdef __cplusplus
extern "C" {
#endif


#if __STDC_VERSION__ >= 199901L
#  define KL_RESTRICT restrict
#elif _MSC_VER
#  define KL_RESTRICT __restrict
#else
#  define KL_RESTRICT 
#endif

#ifdef __cplusplus
}
#endif

#endif