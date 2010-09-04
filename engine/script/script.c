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
#include "script/script.h"
#include "core/logger.h"
#include "core/ringbuffer.h"

#include <swig_autogen.h>

// Extern the lua module loaders
extern int luaopen_scriptevent(lua_State* L);

GC_DECLARE_RINGBUFFER_TYPE(gc_script_event_TEMP);
GC_IMPLEMENT_RINGBUFFER_TYPE(gc_script_event_TEMP);

struct _gc_script_context
{
   lua_State* lua_state;
   gc_ringbuffer(gc_script_event_TEMP) event_buffer;
};

int gc_script_init(gc_script_context* context, size_t event_queue_size)
{     
   // Allocate script context
   struct _gc_script_context* sctx = gc_heap_alloc(sizeof(struct _gc_script_context), 4);

   if(sctx == NULL)
      return GC_ERROR;
      
   // Allocate event buffer
   if(gc_alloc_ringbuffer(gc_script_event_TEMP, &sctx->event_buffer, event_queue_size) != GC_SUCCESS)
   {
      gc_heap_free(sctx);
      return GC_ERROR;
   }

   // Start up lua
   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   luaopen_scriptevent(sctx->lua_state);
   LOAD_SWIG_LIBS(sctx->lua_state);
   
   // Assign a global for the script context assigned to this lua state
   lua_pushlightuserdata(sctx->lua_state, sctx);
   lua_setglobal(sctx->lua_state, "SCTX");
   
   // Return
   (*context) = sctx;
   return GC_SUCCESS;
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
   GC_LOGF(GC_LL_ERR, "%s\n", syntax_err);
   lua_pop(state, 1);
}

void _gc_script_run_internal(void* arg)
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
         GC_LOGF(GC_LL_ERR, "Unknown error loading file '%s'", run_arg->file_name);
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
            GC_LOGF(GC_LL_ERR, "Unknown error calling main function.");
            break;
         }
      }
   }
}

int gc_script_run(gc_script_context context, const char* file_name, bool threaded, int argc, const char** argv)
{
   //
   script_run_arg run_args;
   run_args.state = context->lua_state;
   run_args.argc = argc;
   run_args.argv = argv;
   run_args.file_name = file_name;
   
   // Load the file, and if successful, execute
   switch(luaL_loadfile(context->lua_state, file_name))
   {
      case 0:
      {
         if(threaded)
         {
            amp_thread_t script_thread;

            int create_res = amp_thread_create_and_launch(&script_thread, AMP_DEFAULT_ALLOCATOR, 
               &run_args, _gc_script_run_internal);
            
            if(create_res != 0)
               return GC_ERROR;
            
            amp_thread_join_and_destroy(&script_thread, AMP_DEFAULT_ALLOCATOR);
         }
         else
            _gc_script_run_internal(&run_args);
         return GC_SUCCESS;
      }
      
      // Possible errors
      case LUA_ERRFILE:
      {
         GC_LOGF(GC_LL_ERR, "File '%s' not found.\n", file_name);
         return GC_ERROR;
      }
      case LUA_ERRSYNTAX:
      {
         _on_lua_err(context->lua_state);
         return GC_ERROR;
      }
      case LUA_ERRMEM: 
      {
         GC_LOGF(GC_LL_ERR, "lua ran out of memory opening file %s.\n", file_name);
         return GC_ERROR;
      }
      default:
      {
         return GC_ERROR;
      }
   }
}

void gc_script_destroy(gc_script_context* context)
{
   struct _gc_script_context* sctx = *context;

   lua_close(sctx->lua_state);
   gc_free_ringbuffer(gc_script_event_TEMP, &sctx->event_buffer);
   
   gc_heap_free(sctx);
}

int gc_script_event_enqueue(gc_script_context context, const gc_script_event_TEMP* event)
{
   return gc_reserve_ringbuffer(gc_script_event_TEMP, &context->event_buffer, event);
}

int gc_script_event_dequeue(gc_script_context context, gc_script_event_TEMP* event)
{
   return gc_retrieve_ringbuffer(gc_script_event_TEMP, &context->event_buffer, event);
}