/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2011 GameClay LLC
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
#include <FlightlessManicotti/game/scene2d/scene_container_2d.h>
#include <FlightlessManicotti/core/hash.h>
#include "scriptinterface/helpers/vector2d.h"

#define SCENE2D_LIB "Scene2D"
#define SCENEENTITY2D "SceneEntity2D"
extern const char* VECTOR2D_LUA_LIB;

static int Scene2D_new(lua_State* L)
{
   uint32_t max_entries;
   kl_scene_container_2d_t* sctr;
   luaL_argcheck(L, lua_isnumber(L, 1), 1, "expected max scene entities");
   
   sctr = (kl_scene_container_2d_t*)lua_newuserdata(L, sizeof(kl_scene_container_2d_t));
   max_entries = lua_tointeger(L, 1);
   
   if(kl_alloc_scene_container_2d(sctr, KL_DEFAULT_PROCESS_MANAGER, max_entries) == KL_SUCCESS)
   {
      luaL_getmetatable(L, SCENE2D_LIB);
      lua_setmetatable(L, -2);
   }
   else
   {
      lua_pushnil(L);
      luaL_error(L, "Creation of Scene2D failed: kl_alloc_scene_container_2d was not successful.");
   }
   
   // The userdata (kl_scene_container_2d) was put on the stack by the 'lua_newuserdata' call
   return 1;
}

static int Scene2D_gc(lua_State* L)
{
   kl_scene_container_2d_t* sctr = (kl_scene_container_2d_t*)lua_touserdata(L, 1);
   if(sctr != NULL) kl_free_scene_container_2d(sctr);
   return 0;
}

// TODO: Should probably pool these at some point?
typedef struct
{
   uint32_t id;
   kl_scene_container_2d_t scene;
} scene_entity_2d;

static int Scene2D_reserve(lua_State* L)
{
   kl_scene_container_2d_t* sctr;
   scene_entity_2d* entity;
   
   sctr = (kl_scene_container_2d_t*)lua_touserdata(L, 1);
   if(sctr != NULL)
   {
      entity = (scene_entity_2d*)lua_newuserdata(L, sizeof(scene_entity_2d));
      if(entity != NULL && 
         (kl_reserve_scene_container_2d_id(*sctr, &entity->id) == KL_SUCCESS))
      {
         entity->scene = *sctr;
         luaL_getmetatable(L, SCENEENTITY2D);
         lua_setmetatable(L, -2);
         
         lua_pushvalue(L, 2);
         (*sctr)->reference[entity->id] = luaL_ref(L, LUA_REGISTRYINDEX);
         
         return 1;
      }
   }
   
   return 0;
}

static int Scene2D_raycast(lua_State* L)
{
   kl_scene_container_2d_t* sctr;
   kl_raycast_hit_t hit;
   float from[2];
   float to[2];
   float* out_pos;
   uint32_t type;
   
   sctr = (kl_scene_container_2d_t*)lua_touserdata(L, 1);
   if(sctr != NULL)
   {
      lua_readvector2d(L, 2, from); // From {x,y}
      lua_readvector2d(L, 3, to);   // To {x,y}
      type = lua_tointeger(L, 4);   // Type
      
      if(kl_raycast_scene_container_2d(*sctr, from, to, type, &hit) > 0)
      {
         lua_rawgeti(L, LUA_REGISTRYINDEX, (*sctr)->reference[hit.id]);
         
         out_pos = (float*)lua_newuserdata(L, sizeof(float) * 2);
         if(out_pos != NULL)
         {
            out_pos[0] = hit.position[0];
            out_pos[1] = hit.position[1];
            luaL_getmetatable(L, VECTOR2D_LUA_LIB);
            lua_setmetatable(L, -2);
            return 2;
         }
         return 1;
      }
   }
   
   return 0;
}

static int SceneEntity2D_index(lua_State* L)
{
   const char* key;
   size_t key_len;
   uint32_t hash;
   scene_entity_2d* entity;
   
   key = lua_tolstring(L, 2, &key_len);
   
   // No valid keys have len > 12, and this avoids the loops inside the hash
   // function. See 'core/lookup3.c' for hash implementation.
   if(key_len <= 12)
   {
      entity = (scene_entity_2d*)lua_touserdata(L, 1);
      
      hash = kl_hash(key, key_len, 0);
      //KL_LOGF(KL_LL_NRM, "Indexing key: %s, hash: 0x%08X\n", key, hash);
      switch(hash)
      {
         case 0xA4731157: // 'position'
         {
            lua_pushlightuserdata(L, &entity->scene->pos_xy[entity->id * 2]);
            luaL_getmetatable(L, VECTOR2D_LUA_LIB);
            lua_setmetatable(L, -2);
            return 1;
         }
         
         case 0x2F6488D6: // 'type'
         case 0xFC45D41D: // 'typemask'
         {
            lua_pushinteger(L, entity->scene->typemask[entity->id]);
            return 1;
         }
         
         case 0xC8EB1006: // 'radius'
         {
            lua_pushnumber(L, entity->scene->radius[entity->id]);
            return 1;
         }
      }
   }
   
   return 0;
}

static int SceneEntity2D_newindex(lua_State* L)
{
   const char* key;
   size_t key_len;
   uint32_t hash;
   scene_entity_2d* entity;
   
   key = lua_tolstring(L, 2, &key_len);
   
   // No valid keys have len > 12, and this avoids the loops inside the hash
   // function. See 'core/lookup3.c' for hash implementation.
   if(key_len <= 12)
   {
      entity = (scene_entity_2d*)lua_touserdata(L, 1);
      
      hash = kl_hash(key, key_len, 0);
      
      switch(hash)
      {
         case 0xA4731157: // 'position'
         {
            lua_readvector2d(L, 3, &entity->scene->pos_xy[entity->id * 2]);
            return 0;
         }
         
         case 0x2F6488D6: // 'type'
         case 0xFC45D41D: // 'typemask'
         {
            // Must maintain reserved bit, otherwise bad things will happen
            entity->scene->typemask[entity->id] = (lua_tointeger(L, 3) | SceneType_Reserved);
            return 0;
         }
         
         case 0xC8EB1006: // 'radius'
         {
            entity->scene->radius[entity->id] = lua_tonumber(L, 3);
            return 1;
         }
      }
   }
   
   return 0;
}

static int SceneEntity2D_gc(lua_State* L)
{
   scene_entity_2d* entity;
   entity = (scene_entity_2d*)lua_touserdata(L, 1);
   
   if(entity != NULL && entity->scene != NULL) 
   {
      luaL_unref(L, LUA_REGISTRYINDEX, entity->scene->reference[entity->id]);
      kl_free_scene_container_2d_id(entity->scene, entity->id);
      entity->scene = NULL;
   }
   
   return 0;
}

static const struct luaL_reg Scene2D_instance_methods [] = {
   {"reserve", Scene2D_reserve},
   {"raycast", Scene2D_raycast},
   {NULL, NULL}
};

static const struct luaL_reg Scene2D_class_methods [] = {
   {"new", Scene2D_new},
   {NULL, NULL}
};

int luaopen_scene2d(lua_State* L)
{
   // Scene2D
   luaL_newmetatable(L, SCENE2D_LIB);
   luaL_register(L, 0, Scene2D_instance_methods);
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, Scene2D_gc);
   lua_setfield(L, -2, "__gc");
   
   luaL_register(L, SCENE2D_LIB, Scene2D_class_methods);
   
   // SceneEntity2D
   luaL_newmetatable(L, SCENEENTITY2D);
   lua_pushcfunction(L, SceneEntity2D_index);
   lua_setfield(L, -2, "__index");
   lua_pushcfunction(L, SceneEntity2D_newindex);
   lua_setfield(L, -2, "__newindex");
   lua_pushcfunction(L, SceneEntity2D_gc);
   lua_setfield(L, -2, "__gc");
   
   return 1;
}