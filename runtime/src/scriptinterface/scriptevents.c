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

#include <FlightlessManicotti/scriptinterface/scriptevents.h>
#include <FlightlessManicotti/core/hash.h>
#include <string.h>

uint32_t g_script_event_context_type[1024] = {0}; /* This is pretty hacky */

uint32_t kl_register_script_event(const char* name)
{
   uint32_t ret = kl_hash(name, strlen(name), 0);
   /*KL_LOGF(KL_LL_NRM, "-Registering event: %s as %u\n", name, ret);*/
   /* TODO: track allocated ids in debug? */
   return ret;
}

void kl_register_script_event_context_type(uint32_t id, uint32_t context_type)
{
   g_script_event_context_type[id % 1024] = context_type;
}

extern KL_API uint32_t kl_get_script_event_context_type(uint32_t id)
{
   return g_script_event_context_type[id % 1024];
}
