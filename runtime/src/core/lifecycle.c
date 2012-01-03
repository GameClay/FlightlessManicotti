/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2011 GameClay LLC
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
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/scriptinterface/script.h>
#include <FlightlessManicotti/process/process.h>

typedef struct
{
   KL_BOOL initialized;
} kl_runtime_state_t;

kl_runtime_state_t g_runtime_state = { KL_FALSE };

/* KL_DEFAULT_SCRIPT_CONTEXT from scriptinterface/script.c */
extern kl_script_context_t g_script_context;

/* KL_DEFAULT_PROCESS_MANAGER from process/process.c */
extern kl_process_manager_t g_process_manager;

/* kl_init_mainloop from process/mainloop.c */
extern int kl_init_mainloop(const char* main_script, int argc, const char* argv[]);

int kl_initialize(KL_BOOL use_threads, const char* main_script, int argc, const char* argv[])
{
   int ret = KL_ERROR;
   
   KL_ASSERT(!g_runtime_state.initialized, "Runtime already initialized.");
   KL_ASSERT(g_script_context == NULL, "KL_DEFAULT_SCRIPT_CONTEXT already initialized.");
   KL_ASSERT(g_process_manager == NULL, "KL_DEFAULT_PROCESS_MANAGER already initialized.");
   
   /* TODO: Don't hard code ring-buffer size */
   ret = kl_script_init(&g_script_context, use_threads, 1 << 10);
   
   /* TODO: Growable process list size */
   ret |= kl_alloc_process_manager(&g_process_manager, 128);
   
   /* Let the "main loop" do initialization */
   if(ret == KL_SUCCESS)
      ret = kl_init_mainloop(main_script, argc, argv);
   
   g_runtime_state.initialized = (ret == KL_SUCCESS ? KL_TRUE : KL_FALSE);
   return ret;
}

void kl_destroy()
{
   KL_ASSERT(g_runtime_state.initialized, "Runtime not initialized.");
   kl_script_destroy(&g_script_context);
   kl_free_process_manager(&g_process_manager);
   g_script_context = NULL;
}
