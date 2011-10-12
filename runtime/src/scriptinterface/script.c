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
 
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <amp/amp.h>
#include <FlightlessManicotti/scriptinterface/script.h>
#include <FlightlessManicotti/core/logger.h>
#include <FlightlessManicotti/core/ringbuffer.h>
#include <FlightlessManicotti/core/simd.h>
#include "swig_autogen.h"

// Extern the lua module loaders
extern int luaopen_scriptevents(lua_State* L);
extern int luaopen_lsqlite3(lua_State* L);
extern int luaopen_cast(lua_State* L);
extern int luaopen_scene2d(lua_State* L);
extern int luaopen_vector2d(lua_State* L);

struct _kl_script_context
{
   lua_State* lua_state;
   kl_ringbuffer_t(kl_int32x4_t) event_buffer;
   KL_BOOL threaded;
   KL_BOOL keep_running;
   amp_thread_t thread;
   
   int argc;
   const char** argv;
   const char* file_name;

   int event_handler_ref;
};

// KL_DEFAULT_SCRIPT_CONTEXT
kl_script_context_t g_script_context = NULL;

// script.events.EOF
kl_script_event_t g_event_EOF;

int kl_script_init(kl_script_context_t* context, KL_BOOL threaded, size_t event_queue_max)
{
   struct _kl_script_context* sctx = NULL;
   
   KL_ASSERT(context != NULL, "Cannot use KL_DEFAULT_SCRIPT_CONTEXT during allocation.");
   if(context == NULL)
      return KL_ERROR;
      
   // Allocate script context
   sctx = (struct _kl_script_context*)kl_heap_alloc(sizeof(struct _kl_script_context));

   if(sctx == NULL)
      return KL_ERROR;
      
   // Allocate event buffer
   if(kl_alloc_ringbuffer(kl_int32x4_t, &sctx->event_buffer, event_queue_max * sizeof(kl_int32x4_t)) != KL_SUCCESS)
   {
      kl_heap_free(sctx);
      return KL_ERROR;
   }
   
   // Set threaded or not
   sctx->threaded = threaded;
   sctx->thread = NULL;
   sctx->keep_running = KL_TRUE;
   
   // No event handler ref yet
   sctx->event_handler_ref = 0;
   
   // Set up EOF event
   g_event_EOF.event.id = kl_register_script_event("EOF");
   g_event_EOF.event.context.as_64 = 0;
   g_event_EOF.event.arg = 0;
   
   // Start up lua
   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   
   luaopen_scriptevents(sctx->lua_state);
   luaopen_lsqlite3(sctx->lua_state);
   luaopen_cast(sctx->lua_state);
   luaopen_scene2d(sctx->lua_state);
   luaopen_vector2d(sctx->lua_state);
   
   LOAD_SWIG_LIBS(sctx->lua_state);
   
   // Assign a global for the script context assigned to this lua state
   lua_pushlightuserdata(sctx->lua_state, sctx);
   lua_setglobal(sctx->lua_state, "SCTX");
   
   // Return
   (*context) = sctx;
   return KL_SUCCESS;
}

void _on_lua_err(lua_State* state)
{
   size_t str_sz;
   int top_idx = lua_gettop(state);
   const char* syntax_err = lua_tolstring(state, top_idx, &str_sz);
   KL_LOGF(KL_LL_ERR, "%s\n", syntax_err);
   lua_pop(state, 1);
}

void _kl_script_run_internal(void* arg)
{
   kl_script_context_t sctx = (kl_script_context_t)arg;
   
   // Execute the script file
   switch(lua_pcall(sctx->lua_state, 0, 0, 0))
   {
      case 0: break;
      
      // Runtime error
      case LUA_ERRRUN:
      {
         _on_lua_err(sctx->lua_state);
         break;
      }
      
      default:
      {
         KL_LOGF(KL_LL_ERR, "Unknown error loading file '%s'", sctx->file_name);
      }
   }
   
   // Push function name onto lua stack
   lua_getglobal(sctx->lua_state, "main");
   
   // If there is no 'main' function, we are done.
   if(lua_isnil(sctx->lua_state, -1))
      lua_pop(sctx->lua_state, 1);
   else
   {
      // Load argv onto the lua stack
      int i;
      for(i = 0; i < sctx->argc; i++)
      {
         lua_pushstring(sctx->lua_state, sctx->argv[i]);
      }
      
      // Invoke the main function
      switch(lua_pcall(sctx->lua_state, sctx->argc, 0, 0))
      {
         case 0:
         {
            // Threaded execution, so run the message pump.
            if(sctx->threaded)
            {
               while(sctx->keep_running == KL_TRUE)
               {
                  sctx->keep_running &= (kl_script_event_pump(sctx) == KL_SUCCESS);
               }
            }
            break;
         }

         // Runtime error
         case LUA_ERRRUN:
         {
            _on_lua_err(sctx->lua_state);
            break;
         }

         default:
         {
            KL_LOGF(KL_LL_ERR, "Unknown error calling main function.");
            break;
         }
      }
   }
}

int kl_script_run(kl_script_context_t context, const char* file_name, int argc, const char** argv)
{  
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");

   sctx->argc = argc;
   sctx->argv = argv;
   sctx->file_name = file_name;
   
   // Load the file, and if successful, execute
   switch(luaL_loadfile(sctx->lua_state, file_name))
   {
      case 0:
      {
         if(sctx->threaded)
         {
            int create_res = amp_thread_create_and_launch(&sctx->thread, AMP_DEFAULT_ALLOCATOR, 
               sctx, _kl_script_run_internal);
            
            if(create_res != 0)
               return KL_ERROR;
         }
         else
            _kl_script_run_internal(sctx);
         return KL_SUCCESS;
      }
      
      // Possible errors
      case LUA_ERRFILE:
      {
         KL_LOGF(KL_LL_ERR, "File '%s' not found.\n", file_name);
         return KL_ERROR;
      }
      case LUA_ERRSYNTAX:
      {
         _on_lua_err(sctx->lua_state);
         return KL_ERROR;
      }
      case LUA_ERRMEM: 
      {
         KL_LOGF(KL_LL_ERR, "lua ran out of memory opening file %s.\n", file_name);
         return KL_ERROR;
      }
      default:
      {
         return KL_ERROR;
      }
   }
}

void kl_script_destroy(kl_script_context_t* context)
{
   struct _kl_script_context* sctx = NULL;
   
   KL_ASSERT(context != NULL, "Cannot use KL_DEFAULT_SCRIPT_CONTEXT during allocation.");
   if(context != NULL)
   {  
      sctx = *context;
      
      // Stop threading
      if(sctx->threaded)
      {
         KL_ASSERT(sctx->thread != NULL, "Contex marked as threaded, but no thread found.");
         sctx->keep_running = KL_FALSE;
         amp_thread_join_and_destroy(&sctx->thread, AMP_DEFAULT_ALLOCATOR);
         sctx->thread = NULL;
      }
      
      lua_close(sctx->lua_state);
      kl_free_ringbuffer(kl_int32x4_t, &sctx->event_buffer);
   
      kl_heap_free(sctx);
   }
}

int kl_script_event_enqueue(kl_script_context_t context, const kl_script_event_t* event)
{
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   return kl_reserve_ringbuffer(kl_int32x4_t, &sctx->event_buffer, &event->as_int32x4);
}

int kl_script_event_dequeue(kl_script_context_t context, kl_script_event_t* event)
{
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   return kl_retrieve_ringbuffer(kl_int32x4_t, &sctx->event_buffer, &event->as_int32x4);
}

KL_BOOL kl_script_is_threaded(kl_script_context_t context)
{
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   return sctx->threaded;
}

int kl_script_event_endframe(kl_script_context_t context, kl_script_event_fence_t* fence)
{
   kl_script_event_t eof_evt;
   kl_int32x4_t xmm0 = kl_load_int32x4(&g_event_EOF.as_int32x4);
   kl_store_int32x4(&eof_evt.as_int32x4, xmm0);
   
   if(fence != NULL)
   {
      eof_evt.event.context.as_ptr = fence;
      fence->processed = KL_FALSE;
   }

   return kl_script_event_enqueue(context, &eof_evt);
}

int kl_script_event_fence_wait(const kl_script_event_fence_t* fence)
{
   return (fence->processed == KL_TRUE ? KL_SUCCESS : KL_RETRY);
}

int kl_script_event_fence_notify(kl_script_event_fence_t* fence)
{
   KL_ASSERT(fence->processed == KL_FALSE, "Fence has already been notified.");
   fence->processed = KL_TRUE;
   return KL_SUCCESS;
}

int kl_script_event_pump(kl_script_context_t context)
{
   int ret = KL_ERROR;
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   
   // Skip this if we already have a ref to the handler function
   // TODO: Currently no way to change handler. Should this stay the case?
   if(sctx->event_handler_ref == 0)
   {
      // Push function name onto lua stack, and invoke message handler if it exists
      lua_getglobal(sctx->lua_state, "Events");
      KL_ASSERT(!lua_isnil(sctx->lua_state, -1), "Could not find 'Events' table. This is very bad.");
      if(lua_isnil(sctx->lua_state, -1))
         lua_pop(sctx->lua_state, 1);
      else
      {
         // Grab the 'handler' value
         lua_getfield(sctx->lua_state, -1, "handler");
         KL_ASSERT(lua_isfunction(sctx->lua_state, -1), "Value for 'Events.handler' was not a function.");

         // Pop/store the handler
         sctx->event_handler_ref = luaL_ref(sctx->lua_state, LUA_REGISTRYINDEX);
         
         // Clean up the stack
         lua_pop(sctx->lua_state, 2);
      }
   }

   // Push the function ref back onto the stack
   lua_rawgeti(sctx->lua_state, LUA_REGISTRYINDEX, sctx->event_handler_ref);
   KL_ASSERT(lua_isfunction(sctx->lua_state, -1), "Stored value for 'Events.handler' was not a function.");

   // Invoke event handler
   if(!lua_isfunction(sctx->lua_state, -1))
   {
      KL_ASSERT(KL_FALSE, "Stored value for 'Events.handler' was not a function.");
      lua_pop(sctx->lua_state, 1);
   }
   else
   {
      switch(lua_pcall(sctx->lua_state, 0, 0, 0))
      {
         case 0: 
         {
            ret = KL_SUCCESS;
            break;
         }

         // Runtime error
         case LUA_ERRRUN:
         {
            _on_lua_err(sctx->lua_state);
            break;
         }

         default:
         {
            KL_LOGF(KL_LL_ERR, "Unknown error invoking kl_script_event_pump().\n");
            break;
         }
      }
   }
   
   return ret;
}