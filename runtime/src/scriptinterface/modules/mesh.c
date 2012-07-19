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
#include <FlightlessManicotti/render/opengl/gl_render.h>
#include "scriptinterface/helpers/readers.h"

const char* MESH_LUA_LIB = "Mesh";
int luaopen_mesh(lua_State* L);
int Mesh_pushexisting(lua_State* L, kl_mesh_t* mesh);

extern int push_lua_float_array(lua_State* L, float* a, size_t sz);
extern int push_lua_uint16_array(lua_State* L, uint16_t* a, size_t sz);
extern int push_lua_vector3_array(lua_State* L, float* a, size_t sz);
extern int push_lua_vector2_array(lua_State* L, float* a, size_t sz);

extern kl_render_context_t g_script_render_context;

int Mesh_pushexisting(lua_State* L, kl_mesh_t* mesh)
{
   kl_lua_mesh_t* mesh_holder = lua_newuserdata(L, sizeof(kl_lua_mesh_t));
   mesh_holder->mesh = mesh;
   mesh_holder->auto_dealloc = 0;

   luaL_getmetatable(L, MESH_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int Mesh_new(lua_State* L)
{
   kl_lua_mesh_t* mesh_holder = lua_newuserdata(L, sizeof(kl_lua_mesh_t));
   kl_mesh_t* mesh = kl_heap_alloc(sizeof(kl_mesh_t));
   mesh_holder->mesh = mesh;
   mesh_holder->auto_dealloc = 1;

   memset(mesh, 0, sizeof(kl_mesh_t));
   kl_render_resource_lock(g_script_render_context);
   kl_mesh_init(mesh);
   kl_render_resource_unlock(g_script_render_context);

   luaL_getmetatable(L, MESH_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int Mesh_gc(lua_State* L)
{
   kl_lua_mesh_t* mesh_holder = lua_touserdata(L, 1);
   kl_mesh_t* mesh = mesh_holder->mesh;
   if(mesh != NULL && mesh_holder->auto_dealloc)
   {
      kl_render_resource_lock(g_script_render_context);
      kl_mesh_deinit(mesh);
      kl_render_resource_unlock(g_script_render_context);

      kl_heap_free(mesh->vertex);
      kl_heap_free(mesh->normal);
      kl_heap_free(mesh->tex0);
      kl_heap_free(mesh->col0);
      kl_heap_free(mesh->index);
      kl_heap_free(mesh);
   }
   return 0;
}

static int Mesh_update(lua_State* L)
{
   kl_mesh_t* mesh = NULL;

   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected update mask");
   luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected dynamic mask");

   mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   if(mesh != NULL)
   {
      uint32_t update_mask = (uint32_t)lua_tointeger(L, 2);
      uint32_t dynamic_mask = (uint32_t)lua_tointeger(L, 3);

      kl_render_resource_lock(g_script_render_context);
      kl_mesh_buffer_data(mesh, update_mask, dynamic_mask);
      kl_render_resource_unlock(g_script_render_context);
   }

   return 0;
}

static int Mesh_computenormals(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   if(mesh != NULL && mesh->vertex != NULL)
   {
      if(mesh->normal == NULL)
      {
         mesh->normal = kl_heap_alloc(sizeof(float) * 3 * mesh->num_verts);
      }

      if(mesh->face_normal == NULL)
      {
         /* Triangle list, so (num / 3) * 3 = num */
         mesh->face_normal = kl_heap_alloc(sizeof(float) * mesh->num_indices);
      }

      kl_mesh_recompute_normals(mesh, 0, 0);
   }

   return 0;
}

static int Mesh_loadctm(lua_State* L)
{
   kl_mesh_t* mesh = NULL;

   luaL_argcheck(L, lua_isstring(L, 2), 2, "expected mesh name");

   mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   if(mesh != NULL)
   {
      if(kl_mesh_load_ctm(mesh, lua_tostring(L, 2)) != KL_SUCCESS)
      {
         lua_pushstring(L, "error loading mesh");
         lua_error(L);
      }
   }

   return 0;
}

static int Mesh_getpositions(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   if(mesh != NULL)
   {
      if(mesh->vertex != NULL)
      {
         return push_lua_vector3_array(L, mesh->vertex, mesh->num_verts);
      }
   }

   lua_pushnil(L);
   return 1;
}

static int Mesh_setpositions(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of vertices {{x1, y1, z1}, {x2, y2, z2}, ...}");
   if(mesh != NULL)
   {
      int table_sz = luaL_getn(L, 2);
      float* cur_vert = NULL;

      if(mesh->vertex != NULL) kl_heap_free(mesh->vertex);
      mesh->num_verts = table_sz;
      mesh->vertex = kl_heap_alloc(sizeof(float) * 3 * mesh->num_verts);
      cur_vert = mesh->vertex;

      lua_pushnil(L);
      while(lua_next(L, 2))
      {
         lua_readvector3d(L, -1, cur_vert);
         lua_pop(L, 1);
         cur_vert += 3;
      }
      lua_pop(L, 1);
   }

   return 0;
}

static int Mesh_gettexcoords(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   if(mesh != NULL)
   {
      if(mesh->tex0 != NULL)
      {
         return push_lua_vector2_array(L, mesh->tex0, mesh->num_verts);
      }
   }

   lua_pushnil(L);
   return 1;
}

static int Mesh_settexcoords(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of texture coords {{x1, y1}, {x2, y2}, ...}");
   if(mesh != NULL)
   {
      int table_sz = luaL_getn(L, 2);
      float* cur_vert = NULL;

      if(mesh->tex0 != NULL) kl_heap_free(mesh->tex0);
      mesh->num_verts = table_sz;
      mesh->tex0 = kl_heap_alloc(sizeof(float) * 2 * mesh->num_verts);
      cur_vert = mesh->tex0;

      lua_pushnil(L);
      while(lua_next(L, 2))
      {
         lua_readvector2d(L, -1, cur_vert);
         lua_pop(L, 1);
         cur_vert += 2;
      }
      lua_pop(L, 1);
   }

   return 0;
}

static int Mesh_getindices(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   if(mesh != NULL)
   {
      if(mesh->index != NULL)
      {
         return push_lua_uint16_array(L, mesh->index, mesh->num_indices);
      }
   }

   lua_pushnil(L);
   return 1;
}

static int Mesh_setindices(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of indices {i1, i2, i3, ...}");
   if(mesh != NULL)
   {
      int table_sz = luaL_getn(L, 2);
      uint16_t* cur_idx = NULL;

      if(mesh->index != NULL) kl_heap_free(mesh->index);
      mesh->num_indices = table_sz;
      mesh->index = kl_heap_alloc(sizeof(uint16_t) * mesh->num_indices);
      cur_idx = mesh->index;

      lua_pushnil(L);
      while(lua_next(L, 2))
      {
         *cur_idx = (uint16_t)lua_tointeger(L, -1);
         lua_pop(L, 1);
         cur_idx++;
      }
      lua_pop(L, 1);
   }

   return 0;
}

static int Mesh_setfaces(lua_State* L)
{
   kl_mesh_t* mesh = *((kl_mesh_t**)lua_touserdata(L, 1));

   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of faces {{f11, f12, f13}, {f21, f22, f23}, ...}");
   if(mesh != NULL)
   {
      int table_sz = luaL_getn(L, 2);
      uint16_t* cur_idx = NULL;

      if(mesh->index != NULL) kl_heap_free(mesh->index);
      mesh->num_indices = table_sz * 3;
      mesh->index = kl_heap_alloc(sizeof(uint16_t) * 3 * mesh->num_indices);
      cur_idx = mesh->index;

      lua_pushnil(L);
      while(lua_next(L, 2))
      {
         lua_readtriangleface(L, -1, cur_idx);
         lua_pop(L, 1);
         cur_idx += 3;
      }
      lua_pop(L, 1);
   }

   return 0;
}

static const struct luaL_reg Mesh_instance_methods [] = {
   {"update", Mesh_update},
   {"loadctm", Mesh_loadctm},
   {"computenormals", Mesh_computenormals},

   {"getpositions", Mesh_getpositions},
   {"setpositions", Mesh_setpositions},

   {"settexcoords", Mesh_settexcoords},
   {"gettexcoords", Mesh_gettexcoords},

   {"getindices", Mesh_getindices},
   {"setindices", Mesh_setindices},
   {"setfaces", Mesh_setfaces},

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
