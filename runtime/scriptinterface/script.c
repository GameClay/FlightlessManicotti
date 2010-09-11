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
 
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <amp/amp.h>
#include "scriptinterface/script.h"
#include "core/logger.h"
#include "core/ringbuffer.h"
#include "swig_autogen.h"

// Extern the lua module loaders
extern int luaopen_scriptevents(lua_State* L);
extern int luaopen_lsqlite3(lua_State* L);

KL_DECLARE_RINGBUFFER_TYPE(kl_script_event_t);
KL_IMPLEMENT_RINGBUFFER_TYPE(kl_script_event_t);

struct _kl_script_context
{
   lua_State* lua_state;
   kl_ringbuffer_t(kl_script_event_t) event_buffer;
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

int kl_script_init(kl_script_context_t* context, KL_BOOL threaded, size_t event_queue_size)
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
   if(kl_alloc_ringbuffer(kl_script_event_t, &sctx->event_buffer, event_queue_size) != KL_SUCCESS)
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

   // Start up lua
   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   
   luaopen_scriptevents(sctx->lua_state);
   luaopen_lsqlite3(sctx->lua_state);
   
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
                  kl_script_event_pump(sctx);
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
            _kl_script_run_internal(sctx->thread);
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
      kl_free_ringbuffer(kl_script_event_t, &sctx->event_buffer);
   
      kl_heap_free(sctx);
   }
}

int kl_script_event_enqueue(kl_script_context_t context, const kl_script_event_t* event)
{
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   return kl_reserve_ringbuffer(kl_script_event_t, &sctx->event_buffer, event);
}

int kl_script_event_dequeue(kl_script_context_t context, kl_script_event_t* event)
{
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   return kl_retrieve_ringbuffer(kl_script_event_t, &sctx->event_buffer, event);
}

KL_BOOL kl_script_is_threaded(kl_script_context_t context)
{
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   KL_ASSERT(sctx, "NULL context.");
   return sctx->threaded;
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
      lua_getglobal(sctx->lua_state, "script");
      KL_ASSERT(!lua_isnil(sctx->lua_state, -1), "Could not find 'script' table. This is very bad.");
      if(lua_isnil(sctx->lua_state, -1))
         lua_pop(sctx->lua_state, 1);
      else
      {
         // Grab the 'events' table
         lua_pushstring(sctx->lua_state, "events");
         lua_gettable(sctx->lua_state, -2);
         KL_ASSERT(!lua_isnil(sctx->lua_state, -1), "Could not find 'script.events' table. This is very bad.");

         // Grab the 'handler' value
         lua_getfield(sctx->lua_state, -1, "handler");
         KL_ASSERT(lua_isfunction(sctx->lua_state, -1), "Value for 'events.script.handler' was not a function.");

         // Pop/store the handler
         sctx->event_handler_ref = luaL_ref(sctx->lua_state, LUA_REGISTRYINDEX);
         
         // Clean up the stack
         lua_pop(sctx->lua_state, 2);
      }
   }

   // Push the function ref back onto the stack
   lua_rawgeti(sctx->lua_state, LUA_REGISTRYINDEX, sctx->event_handler_ref);
   KL_ASSERT(lua_isfunction(sctx->lua_state, -1), "Stored value for 'events.script.handler' was not a function.");

   // Invoke event handler
   if(!lua_isfunction(sctx->lua_state, -1))
   {
      KL_ASSERT(KL_FALSE, "Stored value for 'events.script.handler' was not a function.");
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
            KL_LOGF(KL_LL_ERR, "Unknown error invoking script.events.pump().");
            break;
         }
      }
   }
   
   return ret;
}