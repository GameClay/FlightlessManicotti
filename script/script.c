#include <lua.h>
#include <lauxlib.h>

#include "script/script.h"
#include "core/memory.h"

struct _gc_script_context
{
   lua_State* lua_state;
};

int gc_script_init(gc_script_context* context)
{
   struct _gc_script_context* sctx = gc_heap_alloc(sizeof(struct _gc_script_context), 16);

   if(sctx == NULL)
      return GC_ERROR;

   printf("Initializing Lua...\n");
   sctx->lua_state = lua_open();
   luaL_openlibs(sctx->lua_state);
   
   (*context) = sctx;
   
   // Hax
   luaL_dofile(sctx->lua_state, "script.lua");

   return GC_SUCCESS;
}

void gc_script_destroy(gc_script_context* context)
{
   struct _gc_script_context* sctx = *context;
   
   printf("Destroying Lua...\n");
   lua_close(sctx->lua_state);
   
   gc_heap_free(sctx);
}