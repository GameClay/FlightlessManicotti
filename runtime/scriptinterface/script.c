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
extern int luaopen_scriptevent(lua_State* L);
extern int luaopen_lsqlite3(lua_State* L);

KL_DECLARE_RINGBUFFER_TYPE(kl_script_event_t);
KL_IMPLEMENT_RINGBUFFER_TYPE(kl_script_event_t);

struct _kl_script_context
{
   lua_State* lua_state;
   kl_ringbuffer_t(kl_script_event_t) event_buffer;
};

// KL_DEFAULT_SCRIPT_CONTEXT
kl_script_context_t g_script_context = NULL;

int kl_script_init(kl_script_context_t* context, size_t event_queue_size)
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

   // Start up lua
   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   
   luaopen_scriptevent(sctx->lua_state);
   luaopen_lsqlite3(sctx->lua_state);
   
   LOAD_SWIG_LIBS(sctx->lua_state);
   
   // Assign a global for the script context assigned to this lua state
   lua_pushlightuserdata(sctx->lua_state, sctx);
   lua_setglobal(sctx->lua_state, "SCTX");
   
   // Return
   (*context) = sctx;
   return KL_SUCCESS;
}

typedef struct
{
   lua_State* state;
   int argc;
   const char** argv;
   const char* file_name;
} script_run_arg;

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
   script_run_arg* run_arg = (script_run_arg*)arg;
   
   // Execute the script file
   switch(lua_pcall(run_arg->state, 0, 0, 0))
   {
      case 0: break;
      
      // Runtime error
      case LUA_ERRRUN:
      {
         _on_lua_err(run_arg->state);
         break;
      }
      
      default:
      {
         KL_LOGF(KL_LL_ERR, "Unknown error loading file '%s'", run_arg->file_name);
      }
   }
   
   // Push function name onto lua stack
   lua_getglobal(run_arg->state, "main");
   
   // If there is no 'main' function, we are done.
   if(lua_isnil(run_arg->state, -1))
      lua_pop(run_arg->state, 1);
   else
   {
      // Load argv onto the lua stack
      int i;
      for(i = 0; i < run_arg->argc; i++)
      {
         lua_pushstring(run_arg->state, run_arg->argv[i]);
      }
      
      // Invoke the main function
      switch(lua_pcall(run_arg->state, run_arg->argc, 0, 0))
      {
         case 0: break;

         // Runtime error
         case LUA_ERRRUN:
         {
            _on_lua_err(run_arg->state);
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

int kl_script_run(kl_script_context_t context, const char* file_name, KL_BOOL threaded, int argc, const char** argv)
{  
   
   script_run_arg run_args;
   run_args.state = NULL;
   run_args.argc = argc;
   run_args.argv = argv;
   run_args.file_name = file_name;
   
   kl_script_context_t sctx = (context == KL_DEFAULT_SCRIPT_CONTEXT ? g_script_context : context);
   
   KL_ASSERT(sctx, "NULL context.");
   run_args.state = sctx->lua_state;
   
   // Load the file, and if successful, execute
   switch(luaL_loadfile(sctx->lua_state, file_name))
   {
      case 0:
      {
         if(threaded)
         {
            amp_thread_t script_thread;

            int create_res = amp_thread_create_and_launch(&script_thread, AMP_DEFAULT_ALLOCATOR, 
               &run_args, _kl_script_run_internal);
            
            if(create_res != 0)
               return KL_ERROR;
            
            amp_thread_join_and_destroy(&script_thread, AMP_DEFAULT_ALLOCATOR);
         }
         else
            _kl_script_run_internal(&run_args);
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