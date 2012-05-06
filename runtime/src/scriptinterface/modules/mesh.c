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

#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <FlightlessManicotti/render/mesh/mesh.h>
#include <FlightlessManicotti/render/render.h>
#include "render/opengl/gl_render.h"

const char* MESH_LUA_LIB = "Mesh";
extern const char* FLOAT_ARRAY_LUA_LIB;
extern const char* UINT16_ARRAY_LUA_LIB;

extern kl_render_context_t g_script_render_context;

extern kl_mesh_t* g_hax_script_mesh; /* Hax */
static int Mesh_setashaxmesh(lua_State* L)
{
   kl_mesh_t* mesh = (kl_mesh_t*)lua_touserdata(L, 1);
   if(mesh != NULL)
   {
      g_hax_script_mesh = mesh;
   }

   return 0;
}

static int Mesh_new(lua_State* L)
{
   kl_mesh_t* mesh = (kl_mesh_t*)lua_newuserdata(L, sizeof(kl_mesh_t));

   memset(mesh, 0, sizeof(kl_mesh_t));
   CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
   CGLLockContext(g_script_render_context->resourceCGLContext);
   kl_mesh_init(mesh);
   CGLUnlockContext(g_script_render_context->resourceCGLContext);

   luaL_getmetatable(L, MESH_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int Mesh_gc(lua_State* L)
{
   kl_mesh_t* mesh = (kl_mesh_t*)lua_touserdata(L, 1);
   if(mesh != NULL)
   {
      CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
      CGLLockContext(g_script_render_context->resourceCGLContext);
      kl_mesh_deinit(mesh);
      CGLUnlockContext(g_script_render_context->resourceCGLContext);

      kl_heap_free(mesh->vertex);
      kl_heap_free(mesh->normal);
      kl_heap_free(mesh->tex0);
      kl_heap_free(mesh->col0);
      kl_heap_free(mesh->index);
   }
   return 0;
}

static int Mesh_reserve(lua_State* L)
{
   kl_mesh_t* mesh = NULL;

   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected number of vertices to reserve");
   luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected number of indices to reserve");

   mesh = (kl_mesh_t*)lua_touserdata(L, 1);
   if(mesh != NULL)
   {
      mesh->num_verts = lua_tonumber(L, 2);
      mesh->vertex = kl_heap_alloc(sizeof(float) * 3 * mesh->num_verts);
      mesh->normal = kl_heap_alloc(sizeof(float) * 3 * mesh->num_verts);
      mesh->tex0 = kl_heap_alloc(sizeof(float) * 2 * mesh->num_verts);
      mesh->col0 = kl_heap_alloc(sizeof(uint32_t) * mesh->num_verts);

      mesh->num_indices = lua_tonumber(L, 3);
      mesh->index = kl_heap_alloc(sizeof(uint16_t) * mesh->num_indices);
   }

   return 0;
}

static int Mesh_update(lua_State* L)
{
   kl_mesh_t* mesh = NULL;

   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected update mask");
   luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected dynamic mask");

   mesh = (kl_mesh_t*)lua_touserdata(L, 1);

   if(mesh != NULL)
   {
      uint32_t update_mask = lua_tonumber(L, 2);
      uint32_t dynamic_mask = lua_tonumber(L, 3);

      CGLSetCurrentContext(g_script_render_context->resourceCGLContext);
      CGLLockContext(g_script_render_context->resourceCGLContext);
      kl_mesh_buffer_data(mesh, update_mask, dynamic_mask);
      CGLUnlockContext(g_script_render_context->resourceCGLContext);
   }

   return 0;
}

static int Mesh_getpositions(lua_State* L)
{
   kl_mesh_t* mesh = (kl_mesh_t*)lua_touserdata(L, 1);

   if(mesh->vertex != NULL)
   {
      lua_pushlightuserdata(L, mesh->vertex);
      luaL_getmetatable(L, FLOAT_ARRAY_LUA_LIB);
      lua_setmetatable(L, -2);
   }
   else
   {
      lua_pushnil(L);
   }

   return 1;
}

static int Mesh_getindices(lua_State* L)
{
   kl_mesh_t* mesh = (kl_mesh_t*)lua_touserdata(L, 1);

   if(mesh->vertex != NULL)
   {
      lua_pushlightuserdata(L, mesh->index);
      luaL_getmetatable(L, UINT16_ARRAY_LUA_LIB);
      lua_setmetatable(L, -2);
   }
   else
   {
      lua_pushnil(L);
   }

   return 1;
}

static const struct luaL_reg Mesh_instance_methods [] = {
   {"reserve", Mesh_reserve},
   {"update", Mesh_update},
   {"getpositions", Mesh_getpositions},
   {"getindices", Mesh_getindices},

   {"setashaxmesh", Mesh_setashaxmesh},

   {NULL, NULL}
};

static const struct luaL_reg Mesh_class_methods [] = {
   {"new", Mesh_new},
   {NULL, NULL}
};

int luaopen_mesh(lua_State* L)
{
   luaL_newmetatable(L, MESH_LUA_LIB);
   luaL_register(L, 0, Mesh_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, Mesh_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, MESH_LUA_LIB, Mesh_class_methods);

   /* Constants */
   lua_newtable(L);

   lua_pushnumber(L, kl_mesh_element_vertex);
   lua_setfield(L, -2, "vertex");
   lua_pushnumber(L, kl_mesh_element_normal);
   lua_setfield(L, -2, "normal");
   lua_pushnumber(L, kl_mesh_element_tex0);
   lua_setfield(L, -2, "tex0");
   lua_pushnumber(L, kl_mesh_element_col0);
   lua_setfield(L, -2, "col0");
   lua_pushnumber(L, kl_mesh_element_index);
   lua_setfield(L, -2, "index");
   lua_pushnumber(L, kl_mesh_element_all);
   lua_setfield(L, -2, "all");
   lua_pushnumber(L, 0);
   lua_setfield(L, -2, "none");

   lua_setfield(L, -2, "element");

   return 1;
}
