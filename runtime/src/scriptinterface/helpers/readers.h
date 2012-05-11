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

#ifndef _KL_LUA_READ_HELPERS_H_
#define _KL_LUA_READ_HELPERS_H_

#include <lua.h>
#include <stdint.h>

void lua_readvector2d(lua_State* L, int param_idx, float* out_xy);
void lua_readvector3d(lua_State* L, int param_idx, float* out_xyz);
void lua_readtriangleface(lua_State* L, int param_idx, uint16_t* out_abc);

#endif
