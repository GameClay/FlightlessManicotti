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
 
#ifndef _GC_CCOMPAT_H_
#define _GC_CCOMPAT_H_

#ifdef __cplusplus
extern "C" {
#endif

// http://en.wikipedia.org/wiki/C99
#if __STDC_VERSION__ >= 199901L
   // "inline" is a keyword
   // "restrict" is a keyword
#else
#  ifndef inline
#     define inline static
#  endif

// Visual Studio will have a bad time if we #define restrict __restrict
// because it uses '__declspec(restrict)' in these headers. It's a total
// hack, but include these first, and then redefine 'restrict'
#  ifdef _MSC_VER
#     include <stdlib.h>
#     include <malloc.h>
#  endif

#  ifndef restrict
#     define restrict __restrict
#  endif

#endif

#ifdef __cplusplus
}
#endif

#endif