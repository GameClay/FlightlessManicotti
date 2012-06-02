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

#ifndef _GL_RENDER_H_
#define _GL_RENDER_H_

#include <OpenGL/OpenGL.h> /* Mac */
#include <OpenGL/gl3.h>

#include <FlightlessManicotti/render/shader/shader_manager.h>
#include <FlightlessManicotti/render/shader/effect_manager.h>
#include <FlightlessManicotti/render/render_list.h>

struct _kl_offscreen_target {
   GLuint framebuffer, texture, depthstencil;
   GLuint width, height;
};

struct _kl_render_context
{
   CGLContextObj drawableCGLContext;
   CGLContextObj resourceCGLContext;

   kl_render_list_t* render_list;

   kl_shader_manager_t shader_mgr;
   kl_effect_manager_t effect_mgr;
};

#define KL_SHADER_EFFECT_KEY_SZ 256

struct _kl_shader {
   GLuint shader;
   uint16_t ref_count;
   char effect_key[KL_SHADER_EFFECT_KEY_SZ];
};

struct _kl_effect {
   GLuint program;

   kl_shader_t pixel;
   kl_shader_t geometry;
   kl_shader_t vertex;

   uint16_t ref_count;
   char effect_key[KL_SHADER_EFFECT_KEY_SZ];
};

#endif
