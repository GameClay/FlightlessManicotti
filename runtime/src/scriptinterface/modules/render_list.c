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
#include <FlightlessManicotti/render/render_list.h>
#include "render/opengl/gl_render.h"

extern kl_render_context_t g_script_render_context;

/* Hax */
#include <FlightlessManicotti/math/matrix.h>

const char* RENDER_LIST_LUA_LIB = "RenderList";
const char* RENDER_INSTANCE_LUA_LIB = "RenderInstance";

extern const char* MESH_LUA_LIB;

static int RenderList_new(lua_State* L)
{
   uint32_t list_sz = lua_tointeger(L, 1);
   kl_render_list_t* list = (kl_render_list_t*)lua_newuserdata(L, sizeof(kl_render_list_t));

   list_sz = (list_sz > 0 ? list_sz : 32);

   kl_render_list_init(list, list_sz);

   kl_render_assign_list(g_script_render_context, list);

   luaL_getmetatable(L, RENDER_LIST_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int RenderList_gc(lua_State* L)
{
   kl_render_list_t* list = (kl_render_list_t*)lua_touserdata(L, 1);
   kl_render_list_deinit(list);
   return 0;
}

static int RenderList_insertinst(lua_State* L)
{
   kl_render_list_t* list = (kl_render_list_t*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = (kl_render_instance_t*)luaL_checkudata(L, 2, RENDER_INSTANCE_LUA_LIB);
   kl_render_list_insert_instance(list, inst);
   return 0;
}

static int RenderList_removeinst(lua_State* L)
{
   kl_render_list_t* list = (kl_render_list_t*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = (kl_render_instance_t*)luaL_checkudata(L, 2, RENDER_INSTANCE_LUA_LIB);
   kl_render_list_remove_instance(list, inst);
   return 0;
}

static const struct luaL_reg RenderList_instance_methods [] = {
   {"insert", RenderList_insertinst},
   {"remove", RenderList_removeinst},
   {NULL, NULL}
};

static const struct luaL_reg RenderList_class_methods [] = {
   {"new", RenderList_new},
   {NULL, NULL}
};

/* Render instance */

static int RenderInstance_new(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_newuserdata(L, sizeof(kl_render_instance_t));

   inst->list_index = UINT32_MAX;
   inst->material = NULL;
   inst->mesh = NULL;
   inst->draw_type = GL_TRIANGLES;
   kl_matrix_identity(inst->obj_to_world.m);

   luaL_getmetatable(L, RENDER_INSTANCE_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int RenderInstance_gc(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   if(inst->material != NULL)
   {
      kl_effect_manager_destroy_effect(g_script_render_context, &inst->material);
   }
   return 0;
}

static int RenderInstance_setmesh(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   kl_mesh_t* mesh = (kl_mesh_t*)luaL_checkudata(L, 2, MESH_LUA_LIB);
   inst->mesh = mesh;
   return 0;
}

static int RenderInstance_setmaterial(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   const char* effect_key = lua_tostring(L, 2);

   if(inst->material != NULL)
   {
      kl_effect_manager_destroy_effect(g_script_render_context, &inst->material);
   }

   kl_effect_manager_get_effect(g_script_render_context, effect_key, &inst->material);

   return 0;
}

static int RenderInstance_settransform(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   kl_matrix_identity(inst->obj_to_world.m); /* Hax */
   return 0;
}

static int RenderInstance_setdrawtype(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   inst->draw_type = lua_tointeger(L, 2);
   return 0;
}

static const struct luaL_reg RenderInstance_instance_methods [] = {
   {"setmesh", RenderInstance_setmesh},
   {"setmaterial", RenderInstance_setmaterial},
   {"settransform", RenderInstance_settransform},
   {"setdrawtype", RenderInstance_setdrawtype},
   {NULL, NULL}
};

static const struct luaL_reg RenderInstance_class_methods [] = {
   {"new", RenderInstance_new},
   {NULL, NULL}
};

int luaopen_render_list(lua_State* L)
{
   /* Render list */
   luaL_newmetatable(L, RENDER_LIST_LUA_LIB);
   luaL_register(L, 0, RenderList_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, RenderList_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, RENDER_LIST_LUA_LIB, RenderList_class_methods);

   /* Render instance */
   luaL_newmetatable(L, RENDER_INSTANCE_LUA_LIB);
   luaL_register(L, 0, RenderInstance_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, RenderInstance_gc);
   lua_setfield(L, -2, "__gc");

   luaL_register(L, RENDER_INSTANCE_LUA_LIB, RenderInstance_class_methods);

   /* Draw type constant */
   lua_newtable(L);

   lua_pushnumber(L, GL_POINTS);
   lua_setfield(L, -2, "points");
   lua_pushnumber(L, GL_TRIANGLES);
   lua_setfield(L, -2, "triangles");

   lua_setfield(L, -2, "drawtype");

   return 1;
}