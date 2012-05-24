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

#include <FlightlessManicotti/render/postprocess/fs_quad.h>
#include "../gl_render.h"

struct _kl_fs_quad_renderer {
   GLuint idx_buffer;
   GLuint single_quad_vb;
};

int kl_init_fullscreen_quad_renderer(kl_fs_quad_renderer_t* fs_quad, kl_render_context_t ctx)
{
   return 0;
}

void kl_destroy_fullscreen_quad_renderer(kl_fs_quad_renderer_t* fs_quad)
{

}
