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

#ifndef _KL_SCRIPTINTERFACE_H_
#define _KL_SCRIPTINTERFACE_H_

#include <lua.h>
#include <amp/amp.h>
#include <FlightlessManicotti/scriptinterface/script.h>
#include <FlightlessManicotti/core/ringbuffer.h>

struct _kl_script_context
{
   lua_State* lua_state;
   kl_ringbuffer_t(kl_int32x4_t) event_buffer;
   KL_BOOL threaded;
   KL_BOOL keep_running;
   amp_thread_t thread;

   const char** argv;
   const char* file_name;

   int argc;
   int event_handler_ref;
};

#endif
