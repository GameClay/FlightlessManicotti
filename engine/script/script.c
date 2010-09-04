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

struct _gc_script_context
{
   lua_State* lua_state;
};

int gc_script_init(gc_script_context* context, size_t event_queue_size)
{
   struct _gc_script_context* sctx = gc_heap_alloc(sizeof(struct _gc_script_context), 16);

   if(sctx == NULL)
      return GC_ERROR;

   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   
   (*context) = sctx;
   return GC_SUCCESS;
}

typedef struct
{
   lua_State* state;
   int argc;
   const char** argv;
} script_run_arg;

void _gc_script_run_internal(void* arg)
{
   script_run_arg* run_arg = (script_run_arg*)arg;
   
   // Push function name onto lua stack
   lua_getfield(run_arg->state, LUA_GLOBALSINDEX, "main");
   
   // If there is no 'main' function, pop off that function name
   // and just execute the file
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
   }
   
   lua_call(run_arg->state, 0, 0);
}

int gc_script_run(gc_script_context context, const char* file_name, bool threaded, int argc, const char** argv)
{
   //
   script_run_arg run_args;
   run_args.state = context->lua_state;
   run_args.argc = argc;
   run_args.argv = argv;
   
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
      case LUA_ERRSYNTAX:
      case LUA_ERRMEM: 
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
   
   gc_heap_free(sctx);
}

int gc_script_queue_push(gc_script_context context);
int gc_script_queue_pop(gc_script_context context);