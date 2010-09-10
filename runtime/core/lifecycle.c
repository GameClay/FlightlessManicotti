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
#include "fm.h"
#include "scriptinterface/script.h"

typedef struct
{
   KL_BOOL initialized;
} kl_runtime_state_t;

kl_runtime_state_t g_runtime_state = { KL_FALSE };

// KL_DEFAULT_SCRIPT_CONTEXT from scriptinterface/script.c
extern kl_script_context_t g_script_context;

int kl_initialize()
{
   int ret = KL_ERROR;
   
   KL_ASSERT(!g_runtime_state.initialized, "Runtime already initialized.");
   KL_ASSERT(g_script_context == NULL, "KL_DEFAULT_SCRIPT_CONTEXT already initialized.");
   
   ret = kl_script_init(&g_script_context, 1 << 10);
   
   g_runtime_state.initialized = (ret == KL_SUCCESS ? KL_TRUE : KL_FALSE);
   return ret;
}

void kl_destroy()
{
   KL_ASSERT(g_runtime_state.initialized, "Runtime not initialized.");
   kl_script_destroy(&g_script_context);
   g_script_context = NULL;
}
