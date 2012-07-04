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
int luaopen_render_list(lua_State* L);

extern const char* MESH_LUA_LIB;
extern const char* RENDER_TARGET_LUA_LIB;
extern const char* TEXTURE_LUA_LIB;
extern const char* SHADER_CONSTANT_ASSIGNER_LUA_LIB;
extern const char* SHADER_PROGRAM_LUA_LIB;
extern const char* CONSTANT_BUFFER_LUA_LIB;

/* With no alignment on lua_newuserdata, need to have a holder */
typedef struct lua_render_instance {
   kl_render_instance_t* inst;
} lua_render_instance;

static int RenderList_new(lua_State* L)
{
   uint32_t list_sz = (uint32_t)lua_tointeger(L, 1);
   kl_render_list_t* list = (kl_render_list_t*)lua_newuserdata(L, sizeof(kl_render_list_t));

   list_sz = (list_sz > 0 ? list_sz : 32);

   kl_render_list_init(list, list_sz);

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
   lua_render_instance* inst = (lua_render_instance*)luaL_checkudata(L, 2, RENDER_INSTANCE_LUA_LIB);
   kl_render_list_insert_instance(list, inst->inst);
   return 0;
}

static int RenderList_removeinst(lua_State* L)
{
   kl_render_list_t* list = (kl_render_list_t*)lua_touserdata(L, 1);
   lua_render_instance* inst = (lua_render_instance*)luaL_checkudata(L, 2, RENDER_INSTANCE_LUA_LIB);
   kl_render_list_remove_instance(list, inst->inst);
   return 0;
}

static int RenderList_setdefaulttarget(lua_State* L)
{
   kl_render_list_t* list = (kl_render_list_t*)lua_touserdata(L, 1);
   struct _kl_offscreen_target* target = NULL;

   if(!lua_isnoneornil(L, 2))
   {
      target = (struct _kl_offscreen_target*)luaL_checkudata(L, 2, RENDER_TARGET_LUA_LIB);
   }
   list->default_target = target;
   return 0;
}

static const struct luaL_reg RenderList_instance_methods [] = {
   {"insert", RenderList_insertinst},
   {"remove", RenderList_removeinst},
   {"setdefaulttarget", RenderList_setdefaulttarget},
   {NULL, NULL}
};

static const struct luaL_reg RenderList_class_methods [] = {
   {"new", RenderList_new},
   {NULL, NULL}
};

static int RenderInstance_new(lua_State* L)
{
   lua_render_instance* inst_hldr = (lua_render_instance*)lua_newuserdata(L, sizeof(lua_render_instance));

   kl_render_instance_t* inst = kl_heap_alloc(sizeof(kl_render_instance_t));
   inst_hldr->inst = inst;
   inst->list_index = UINT32_MAX;
   inst->list = NULL;
   inst->effect = NULL;
   inst->mesh = NULL;
   inst->draw_type = GL_TRIANGLES;
   inst->blend_src = GL_ONE;
   inst->blend_dest = GL_ZERO;
   kl_matrix_identity(inst->object_to_world.m);
   inst->consts = NULL;
   inst->num_consts = 0;
   inst->render_target = NULL;
   inst->constant_buffer = NULL;
   inst->draw_buffers[0] = GL_COLOR_ATTACHMENT0;
   inst->num_draw_buffers = 1;
   inst->clear_before_draw = 0;

   luaL_getmetatable(L, RENDER_INSTANCE_LUA_LIB);
   lua_setmetatable(L, -2);

   return 1;
}

static int RenderInstance_gc(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;

   if(inst->list != NULL)
   {
      kl_render_list_remove_instance(inst->list, inst);
   }

   if(inst->consts != NULL)
   {
      uint32_t i;
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

   kl_heap_free(inst);

   return 0;
}

static int RenderInstance_setmesh(lua_State* L)
{
   lua_render_instance* inst = (lua_render_instance*)lua_touserdata(L, 1);
   kl_mesh_t* mesh = (kl_mesh_t*)luaL_checkudata(L, 2, MESH_LUA_LIB);
   inst->inst->mesh = mesh;
   return 0;
}

static int RenderInstance_seteffect(lua_State* L)
{
   lua_render_instance* inst = (lua_render_instance*)lua_touserdata(L, 1);
   struct _kl_effect* effect = (struct _kl_effect*)luaL_checkudata(L, 2, SHADER_PROGRAM_LUA_LIB);

   inst->inst->effect = effect;

   return 0;
}

static void RenderInstance_shaderconsthelper(lua_State* L, kl_shader_constant_t* constant)
{
   if(constant != NULL)
   {
      void* old_ptr = NULL;
      int l_index = lua_gettop(L);

      switch(lua_type(L, l_index))
      {
         case LUA_TNUMBER:
         {
            if(constant->constant_type != KL_SHADER_CONSTANT_TYPE_FLOAT)
            {
               if(constant->dealloc_constant)
               {
                  old_ptr = constant->constant.as_ptr;
               }

               constant->constant.as_float_ptr = kl_heap_alloc(sizeof(float));
            }

            constant->dealloc_constant = 1;
            constant->constant_sz = 1;
            constant->constant_num = 1;
            constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;
            *constant->constant.as_float_ptr = (float)lua_tonumber(L, l_index);
            break;
         }

         case LUA_TTABLE:
         {
            int tsize = 0;
            int csize = 0;

            /* Get size of each element */
            lua_pushnil(L);
            while(lua_next(L, l_index))
            {
               switch(lua_type(L, -1))
               {
                  case LUA_TNUMBER:
                  {
                     if(csize == 0) csize = 1;
                     else
                     {
                        if(csize != 1)
                        {
                           kl_log_err("All elements of a shader constant table assignment must be the same size.");
                        }
                     }
                     break;
                  }

                  case LUA_TTABLE:
                  {
                     if(csize == 0) csize = (int)lua_objlen(L, -1);
                     else
                     {
                        if(csize != (int)lua_objlen(L, -1))
                        {
                           kl_log_err("All elements of a shader constant table assignment must be the same size.");
                        }
                     }
                     break;
                  }
               }
               tsize++;
               lua_pop(L, 1);
            }
            if(tsize == 0) lua_pop(L, 1);

            if(constant->constant_type != KL_SHADER_CONSTANT_TYPE_FLOAT ||
               (constant->constant_num > 0 && constant->constant_num != tsize) ||
               (constant->constant_sz > 0 && constant->constant_sz != csize))
            {
               if(constant->dealloc_constant)
               {
                  old_ptr = constant->constant.as_ptr;
               }

               constant->constant.as_float_ptr = kl_heap_alloc(tsize * csize * sizeof(float));
            }

            constant->constant_num = (uint16_t)tsize;
            constant->constant_sz = (uint16_t)csize;
            constant->dealloc_constant = 1;
            constant->constant_type = KL_SHADER_CONSTANT_TYPE_FLOAT;

            if(tsize > 0)
            {
               float* cur_f = constant->constant.as_float_ptr;
               lua_pushnil(L);
               while(lua_next(L, l_index))
               {
                  switch(lua_type(L, -1))
                  {
                     case LUA_TNUMBER:
                     {
                        *cur_f = (float)lua_tonumber(L, -1);
                        cur_f++;
                        break;
                     }

                     case LUA_TTABLE:
                     {
                        lua_pushnil(L);
                        while(lua_next(L, -2))
                        {
                           *cur_f = (float)lua_tonumber(L, -1);
                           cur_f++;
                           lua_pop(L, 1);
                        }
                        break;
                     }
                  }
                  lua_pop(L, 1);
               }
            }
            break;
         }

         case LUA_TUSERDATA:
         {
            int is_eq = 0;

            /* Check to see if it's a render target */
            lua_getmetatable(L, l_index);
            luaL_getmetatable(L, RENDER_TARGET_LUA_LIB);
            is_eq = lua_equal(L, -1, -2);
            lua_pop(L, 2);
            if(is_eq)
            {
               struct _kl_offscreen_target* target = (struct _kl_offscreen_target*)lua_touserdata(L, l_index);
               constant->dealloc_constant = 0;
               constant->constant_sz = 2; /* 2D texture */
               constant->constant_num = 1;
               constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
               constant->constant.as_tex = target->texture[0]->texture; /* HAX */
               break;
            }

            /* Check to see if it's a texture */
            lua_getmetatable(L, l_index);
            luaL_getmetatable(L, TEXTURE_LUA_LIB);
            is_eq = lua_equal(L, -1, -2);
            lua_pop(L, 2);
            if(is_eq)
            {
               struct _kl_texture* tex = (struct _kl_texture*)lua_touserdata(L, l_index);
               constant->dealloc_constant = 0;
               constant->constant_num = 1;

               if(tex->data_texture)
               {
                  constant->constant_type = KL_SHADER_CONSTANT_TYPE_DATA;
                  constant->constant.as_tex = tex->data_texture;
                  constant->constant_sz = 0;
               }
               else
               {
                  switch(tex->tex_depth)
                  {
                     case GL_TEXTURE_2D:
                     {
                        constant->constant_sz = 2; /* 2D texture */
                        break;
                     }
                  }

                  constant->constant_type = KL_SHADER_CONSTANT_TYPE_TEX;
                  constant->constant.as_tex = tex->texture;
               }
               break;
            }

            /* Check to see if it's an assigner function */
            lua_getmetatable(L, l_index);
            luaL_getmetatable(L, SHADER_CONSTANT_ASSIGNER_LUA_LIB);
            is_eq = lua_equal(L, -1, -2);
            lua_pop(L, 2);
            if(is_eq)
            {
               kl_shader_constant_assigner_t* assigner = (kl_shader_constant_assigner_t*)lua_touserdata(L, l_index);

               constant->dealloc_constant = 0;
               constant->constant_sz = 1;
               constant->constant_num = 1;
               constant->constant_type = KL_SHADER_CONSTANT_TYPE_FN;
               constant->constant.as_fn = assigner->assigner;
               break;
            }
            break;
         }

         default:
         {
            break;
         }
      }

      if(old_ptr != NULL)
      {
         kl_heap_free(old_ptr);
      }
   }
}

static int RenderInstance_setshaderconstants(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   size_t i, num_consts, old_num_consts;
   kl_shader_constant_t** consts;
   kl_shader_constant_t** old_consts;
   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of shader constants");

   num_consts = 0;
   lua_pushnil(L);
   while(lua_next(L, 2))
   {
      lua_pop(L, 1);
      num_consts++;
   }
   if(num_consts == 0) lua_pop(L, 1);

   consts = kl_heap_alloc(sizeof(kl_shader_constant_t*) * num_consts);

   i = 0;
   lua_pushnil(L);
   while(lua_next(L, 2))
   {
      if(lua_isstring(L, -2))
      {
         const char* name;
         lua_pushvalue(L, -2);
         name = lua_tostring(L, -1);
         lua_pop(L, 1);
         consts[i] = kl_heap_alloc(sizeof(kl_shader_constant_t));
         kl_zero_mem(consts[i], sizeof(kl_shader_constant_t));
         strcpy(consts[i]->name, name);
         consts[i]->constant_idx = -1;

         RenderInstance_shaderconsthelper(L, consts[i]);
      }
      else
      {
         consts[i] = kl_heap_alloc(sizeof(kl_shader_constant_t));
         kl_zero_mem(consts[i], sizeof(kl_shader_constant_t));
         consts[i]->constant_idx = -1;
      }

      lua_pop(L, 1);
      i++;
   }
   lua_pop(L, 1);

   old_num_consts = inst->num_consts;
   old_consts = inst->consts;

   CGLLockContext(g_script_render_context->drawableCGLContext);
   inst->num_consts = num_consts;
   inst->consts = consts;
   CGLUnlockContext(g_script_render_context->drawableCGLContext);

   if(old_consts != NULL)
   {
      for(i = 0; i < old_num_consts; i++)
      {
         if(old_consts[i]->dealloc_constant)
         {
            kl_heap_free(old_consts[i]->constant.as_float_ptr);
         }
         kl_heap_free(old_consts[i]);
      }
      kl_heap_free(old_consts);
   }

   return 0;
}


static int RenderInstance_updateshaderconstants(lua_State* L)
{
   size_t i;
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of shader constants");

   lua_pushnil(L);
   while(lua_next(L, 2))
   {
      if(lua_isstring(L, -2))
      {
         kl_shader_constant_t* constant = NULL;
         /* Find the shader constant with this name */
         for(i = 0; i < inst->num_consts; i++)
         {
            size_t name_len;
            const char* name;
            lua_pushvalue(L, -2);
            name = lua_tolstring(L, -1, &name_len);
            lua_pop(L, 1);

            if(strcmp(name, inst->consts[i]->name) == 0)
            {
               constant = inst->consts[i];
               break;
            }
         }

         RenderInstance_shaderconsthelper(L, constant);
      }

      lua_pop(L, 1);
   }
   lua_pop(L, 1);

   return 0;
}

static int RenderInstance_setconstantbuffer(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;

   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table created by ConstantBuffer.new");
   lua_getfield(L, 2, "_constantbuffer");
   if(lua_isnoneornil(L, -1) || !lua_isuserdata(L, -1))
   {
      lua_pop(L, 1);
      luaL_argcheck(L, 0, 2, "expected table created by ConstantBuffer.new");
   }
   else
   {
      inst->constant_buffer = luaL_checkudata(L, -1, CONSTANT_BUFFER_LUA_LIB);
      lua_pop(L, 1);
   }

   return 0;
}

static int RenderInstance_settransform(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   kl_matrix_identity(inst->object_to_world.m); /* Hax */
   return 0;
}

static int RenderInstance_setrendertarget(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
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
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected draw type");
   inst->draw_type = (uint32_t)lua_tointeger(L, 2);
   return 0;
}

static int RenderInstance_setblend(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected blend type");
   luaL_argcheck(L, lua_isnumber(L, 3), 3, "expected blend type");
   inst->blend_src = (uint32_t)lua_tointeger(L, 2);
   inst->blend_dest = (uint32_t)lua_tointeger(L, 3);
   return 0;
}

static int RenderInstance_setclearbeforedraw(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   luaL_argcheck(L, lua_isboolean(L, 2), 2, "expected boolean");
   inst->clear_before_draw = lua_toboolean(L, 2);
   return 0;
}

static int RenderInstance_setdrawbuffers(lua_State* L)
{
   lua_render_instance* inst_hdlr = (lua_render_instance*)lua_touserdata(L, 1);
   kl_render_instance_t* inst = inst_hdlr->inst;
   luaL_argcheck(L, lua_istable(L, 2), 2, "expected table of draw buffer constants");

   inst->num_draw_buffers = 0;
   lua_pushnil(L);
   while(lua_next(L, 2))
   {
      inst->draw_buffers[inst->num_draw_buffers] = (uint32_t)lua_tointeger(L, -1);
      inst->num_draw_buffers++;
      lua_pop(L, 1);
   }
   lua_pop(L, 1);

   return 0;
}

static const struct luaL_reg RenderInstance_instance_methods [] = {
   {"setmesh", RenderInstance_setmesh},
   {"seteffect", RenderInstance_seteffect},
   {"settransform", RenderInstance_settransform},
   {"setdrawtype", RenderInstance_setdrawtype},
   {"setblend", RenderInstance_setblend},
   {"setconstantbuffer", RenderInstance_setconstantbuffer},
   {"setshaderconstants", RenderInstance_setshaderconstants},
   {"updateshaderconstants", RenderInstance_updateshaderconstants},
   {"setrendertarget", RenderInstance_setrendertarget},
   {"setdrawbuffers", RenderInstance_setdrawbuffers},
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
   lua_pushnumber(L, GL_LINE_LOOP);
   lua_setfield(L, -2, "line_loop");

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
