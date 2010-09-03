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
#include <amp/amp.h>
#include "script/script.h"
#include "core/ringbuffer.h"

struct _gc_script_context
{
   gc_ringbuffer event_queue;
   lua_State* lua_state;
};

int gc_script_init(gc_script_context* context, size_t event_queue_size)
{
   struct _gc_script_context* sctx = gc_heap_alloc(sizeof(struct _gc_script_context), 16);

   if(sctx == NULL)
      return GC_ERROR;
   
   if(gc_alloc_ringbuffer(&sctx->event_queue, event_queue_size) != GC_SUCCESS)
   {
      lua_close(sctx->lua_state);
      gc_heap_free(sctx);
      return GC_ERROR;
   }

   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   
   (*context) = sctx;
   return GC_SUCCESS;
}

void _gc_script_run_internal(void* arg)
{
   lua_pcall((lua_State*)arg, 0, LUA_MULTRET, 0);
}

int gc_script_run(gc_script_context context, const char* file_name, bool threaded)
{
   // Load the file, and if successful, execute
   switch(luaL_loadfile(context->lua_state, file_name))
   {
      case 0:
      {
         if(threaded)
         {
            amp_thread_t script_thread;

            int create_res = amp_thread_create_and_launch(&script_thread, AMP_DEFAULT_ALLOCATOR, 
               context->lua_state, _gc_script_run_internal);
            
            if(create_res != 0)
               return GC_ERROR;
            
            amp_thread_join_and_destroy(&script_thread, AMP_DEFAULT_ALLOCATOR);
         }
         else
            _gc_script_run_internal(context->lua_state);
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
   gc_free_ringbuffer(&sctx->event_queue);
   
   gc_heap_free(sctx);
}

int gc_script_queue_push(gc_script_context context);
int gc_script_queue_pop(gc_script_context context);