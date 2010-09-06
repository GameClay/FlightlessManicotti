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
 
#ifndef _GC_LOGGER_H_
#define _GC_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fm.h"

#define GC_LL_NRM 0
#define GC_LL_ERR 1

#define GC_LOGF(ll, fmt, args...) switch(ll) { case GC_LL_ERR: fprintf(stderr, fmt, ## args); break; default: fprintf(stdout, fmt, ## args); }

#ifdef __cplusplus
}
#endif

#endif