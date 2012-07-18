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
#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/scriptinterface/script.h>
#include <FlightlessManicotti/process/process.h>
#include <FlightlessManicotti/render/shader/effect_manager.h>

typedef struct
{
   KL_BOOL initialized;
} kl_runtime_state_t;

kl_runtime_state_t g_runtime_state = { KL_FALSE };

/* KL_DEFAULT_SCRIPT_CONTEXT from scriptinterface/script.c */
extern kl_script_context_t g_script_context;

/* KL_DEFAULT_PROCESS_MANAGER from process/process.c */
extern kl_process_manager_t g_process_manager;

/* KL_DEFAULT_EFFECT_MANAGER from render/opengl/shader/effect_manager.c */
extern kl_effect_manager_t g_effect_manager;

/* kl_init_mainloop from process/mainloop.c */
extern int kl_init_mainloop(const char* main_script, KL_BOOL wait_on_fences, int argc, const char* argv[]);

/* Internal log init/deinit functions */
extern void _kl_log_init();
extern void _kl_log_deinit();

/* script.events.Init */
kl_script_event_t g_event_Init;

/* script.events.Destroy */
kl_script_event_t g_event_Destroy;

int kl_initialize(KL_BOOL use_threads, KL_BOOL wait_on_fence, const char* main_script, int argc, const char* argv[])
{
   int ret = KL_ERROR;

   /* Init logging */
   _kl_log_init();

   KL_ASSERT(!g_runtime_state.initialized, "Runtime already initialized.");
   KL_ASSERT(g_script_context == NULL, "KL_DEFAULT_SCRIPT_CONTEXT already initialized.");
   KL_ASSERT(g_process_manager == NULL, "KL_DEFAULT_PROCESS_MANAGER already initialized.");
   KL_ASSERT(g_effect_manager == NULL, "KL_DEFAULT_EFFECT_MANAGER already initialized.");

   ret = kl_effect_manager_create(&g_effect_manager);

   /* TODO: Don't hard code ring-buffer size */
   ret |= kl_script_init(&g_script_context, use_threads, 1 << 10);

   /* Create Init/Destroy script events */
   if(ret == KL_SUCCESS)
   {
      /* Set up Init event */
      g_event_Init.event.id = kl_register_script_event("Init");
      g_event_Init.event.context.as_64 = 0;
      g_event_Init.event.arg.as_uint32 = 0;

      /* Set up Destroy event */
      g_event_Destroy.event.id = kl_register_script_event("Destroy");
      g_event_Destroy.event.context.as_64 = 0;
      g_event_Destroy.event.arg.as_uint32 = 0;
   }

   /* TODO: Growable process list size */
   ret |= kl_alloc_process_manager(&g_process_manager, 128);

   /* Let the "main loop" do initialization */
   if(ret == KL_SUCCESS)
      ret = kl_init_mainloop(main_script, wait_on_fence, argc, argv);

   /* Send the script init event */
   if(ret == KL_SUCCESS)
   {
      kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &g_event_Init);
   }

   g_runtime_state.initialized = (ret == KL_SUCCESS ? KL_TRUE : KL_FALSE);
   return ret;
}

void kl_predestroy(void)
{
   KL_ASSERT(g_runtime_state.initialized, "Runtime not initialized.");
   kl_script_event_enqueue(KL_DEFAULT_SCRIPT_CONTEXT, &g_event_Destroy);
}

void kl_destroy(void)
{
   KL_ASSERT(g_runtime_state.initialized, "Runtime not initialized.");
   kl_script_destroy(&g_script_context);
   kl_free_process_manager(&g_process_manager);
   kl_effect_manager_destroy(&g_effect_manager);
   g_script_context = NULL;

   /* Destroy logging */
   _kl_log_deinit();
}
