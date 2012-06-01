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

const char* RENDER_LIST_LUA_LIB = "RenderList";
const char* RENDER_INSTANCE_LUA_LIB = "RenderInstance";

extern const char* MESH_LUA_LIB;
extern const char* RENDER_TARGET_LUA_LIB;

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
   inst->blend_src = GL_ONE;
   inst->blend_dest = GL_ZERO;
   kl_matrix_identity(inst->obj_to_world.m);
   inst->consts = NULL;
   inst->num_consts = 0;
   inst->render_target = NULL;
   inst->clear_before_draw = 0;

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

   if(inst->consts != NULL)
   {
      int i;
      for(i = 0; i < inst->num_consts; i++)
      {
         if(inst->consts[i]->dealloc_constant)
         {
            kl_heap_free(inst->consts[i]->constant.as_float_ptr);
         }
         kl_heap_free(inst->consts[i]);
      }
      kl_heap_free(inst->consts);
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
   const char* effect_key;

   luaL_argcheck(L, lua_isstring(L, 2), 2, "expected material identifier");
   effect_key = lua_tostring(L, 2);

   if(inst->material != NULL)
   {
      kl_effect_manager_destroy_effect(g_script_render_context, &inst->material);
   }

   kl_effect_manager_get_effect(g_script_render_context, effect_key, &inst->material);

   return 0;
}

static int RenderInstance_setshaderconstants(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   int i;
   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of shader constants");

   if(inst->consts != NULL)
   {
      for(i = 0; i < inst->num_consts; i++)
      {
         if(inst->consts[i]->dealloc_constant)
         {
            kl_heap_free(inst->consts[i]->constant.as_float_ptr);
         }
         kl_heap_free(inst->consts[i]);
      }
      kl_heap_free(inst->consts);
   }

   i = 0;
   lua_pushnil(L);
   while(lua_next(L, 2))
   {
      lua_pop(L, 1);
      i++;
   }
   if(i == 0) lua_pop(L, 1);

   inst->num_consts = i;
   inst->consts = kl_heap_alloc(sizeof(kl_shader_constant_t*) * inst->num_consts);

   i = 0;
   lua_pushnil(L);
   while(lua_next(L, 2))
   {
      if(lua_isstring(L, -2))
      {
         size_t name_len;
         const char* name;
         lua_pushvalue(L, -2);
         name = lua_tolstring(L, -1, &name_len);
         lua_pop(L, 1);
         inst->consts[i] = kl_heap_alloc(sizeof(kl_shader_constant_t) + name_len);
         kl_zero_mem(inst->consts[i], sizeof(kl_shader_constant_t) + name_len);
         strcpy(inst->consts[i]->name, name);

         switch(lua_type(L, -1))
         {
            case LUA_TNUMBER:
            {
               inst->consts[i]->constant.as_float_ptr = kl_heap_alloc(sizeof(float));
               inst->consts[i]->dealloc_constant = 1;
               inst->consts[i]->constant_sz = 1;
               inst->consts[i]->constant_num = 1;
               inst->consts[i]->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
               *inst->consts[i]->constant.as_float_ptr = lua_tonumber(L, -1);
               break;
            }

            case LUA_TTABLE:
            {
               break;
            }

            case LUA_TUSERDATA:
            {
               int is_eq = 0;

               /* Check to see if it's a render target */
               lua_getmetatable(L, -1);
               luaL_getmetatable(L, RENDER_TARGET_LUA_LIB);
               is_eq = lua_equal(L, -1, -2);
               lua_pop(L, 2);
               if(is_eq)
               {
                  struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, -1);
                  inst->consts[i]->dealloc_constant = 0;
                  inst->consts[i]->constant_sz = 2; /* 2D texture */
                  inst->consts[i]->constant_num = 1;
                  inst->consts[i]->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
                  inst->consts[i]->constant.as_tex = target->texture;
               }
               break;
            }

            /* TODO 
            case LUA_TFUNCTION:
            {
               break;
            } */

            /* TODO
            case LUA_TLIGHTUSERDATA:
            {
               break;
            } */

            default:
            {
               break;
            }
         }
      }
      else
      {
         inst->consts[i] = kl_heap_alloc(sizeof(kl_shader_constant_t));
         kl_zero_mem(inst->consts[i], sizeof(kl_shader_constant_t));
      }

      lua_pop(L, 1);
      i++;
   }
   lua_pop(L, 1);

   return 0;
}

static int RenderInstance_settransform(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   kl_matrix_identity(inst->obj_to_world.m); /* Hax */
   return 0;
}

static int RenderInstance_setrendertarget(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   struct _kl_offscreen_target* target = NULL;

   if(!lua_isnoneornil(L, 2))
   {
      target = (struct _kl_offscreen_target*)luaL_checkudata(L, 2, RENDER_TARGET_LUA_LIB);
   }

   inst->render_target = target;
   return 0;
}

static int RenderInstance_setdrawtype(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected draw type");
   inst->draw_type = lua_tointeger(L, 2);
   return 0;
}

static int RenderInstance_setblend(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected blend type");
   luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected blend type");
   inst->blend_src = lua_tointeger(L, 2);
   inst->blend_dest = lua_tointeger(L, 3);
   return 0;
}

static int RenderInstance_setclearbeforedraw(lua_State* L)
{
   kl_render_instance_t* inst = (kl_render_instance_t*)lua_touserdata(L, 1);
   luaL_argcheck(L, lua_isboolean(L, 2), 2, "expected boolean");
   inst->clear_before_draw = lua_toboolean(L, 2);
   return 0;
}

static const struct luaL_reg RenderInstance_instance_methods [] = {
   {"setmesh", RenderInstance_setmesh},
   {"setmaterial", RenderInstance_setmaterial},
   {"settransform", RenderInstance_settransform},
   {"setdrawtype", RenderInstance_setdrawtype},
   {"setblend", RenderInstance_setblend},
   {"setshaderconstants", RenderInstance_setshaderconstants},
   {"setrendertarget", RenderInstance_setrendertarget},
   {"setclearbeforedraw", RenderInstance_setclearbeforedraw},
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
   lua_pushnumber(L, GL_LINES);
   lua_setfield(L, -2, "lines");
   lua_pushnumber(L, GL_LINE_STRIP);
   lua_setfield(L, -2, "line_strip");

   lua_setfield(L, -2, "drawtype");

   /* Blend type constant */
   lua_newtable(L);

   lua_pushnumber(L, GL_ZERO);
   lua_setfield(L, -2, "zero");
   lua_pushnumber(L, GL_ONE);
   lua_setfield(L, -2, "one");
   lua_pushnumber(L, GL_SRC_COLOR);
   lua_setfield(L, -2, "src_color");
   lua_pushnumber(L, GL_ONE_MINUS_SRC_COLOR);
   lua_setfield(L, -2, "one_minus_src_color");
   lua_pushnumber(L, GL_DST_COLOR);
   lua_setfield(L, -2, "dst_color");
   lua_pushnumber(L, GL_ONE_MINUS_DST_COLOR);
   lua_setfield(L, -2, "one_minus_dst_color");
   lua_pushnumber(L, GL_SRC_ALPHA);
   lua_setfield(L, -2, "src_alpha");
   lua_pushnumber(L, GL_ONE_MINUS_SRC_ALPHA);
   lua_setfield(L, -2, "one_minus_src_alpha");
   lua_pushnumber(L, GL_DST_ALPHA);
   lua_setfield(L, -2, "dst_alpha");
   lua_pushnumber(L, GL_ONE_MINUS_DST_ALPHA);
   lua_setfield(L, -2, "one_minus_dst_alpha");

   lua_setfield(L, -2, "blend");

   return 1;
}
