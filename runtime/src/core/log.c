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

#include <FlightlessManicotti/fm.h>
#include <FlightlessManicotti/core/log.h>
#include <stdarg.h>
#include <stdio.h>

#include "sanskrit/include/sanskrit/sklog.h"

void _kl_log_init(void);
void _kl_log_deinit(void);

static void _kl_default_log_callback(void* context, const char* line, int log_level)
{
   KL_UNUSED(context);
   switch(log_level)
   {
      case KL_LOG_LEVEL_DEBUG: skdebug(line); break;
      case KL_LOG_LEVEL_INFO:  skinfo(line); break;
      case KL_LOG_LEVEL_WARN:  skwarn(line); break;
      case KL_LOG_LEVEL_ERR:   skerr(line); break;
   }
}

static kl_console_line_callback_fn g_console_line_callback = _kl_default_log_callback;
static void* g_console_line_callback_ctx = NULL;

void kl_console_line_callback_assign(kl_console_line_callback_fn callback_fn, void* context)
{
   if(g_console_line_callback == _kl_default_log_callback) sklog_destroy();

   g_console_line_callback = callback_fn;
   g_console_line_callback_ctx = context;
}

void _kl_log_init(void)
{
   if(g_console_line_callback == _kl_default_log_callback) sklog_init();
}

void _kl_log_deinit(void)
{
   if(g_console_line_callback == _kl_default_log_callback) sklog_destroy();
}

static void _kl_write_console_line(const char* line, int log_level)
{
   g_console_line_callback(g_console_line_callback_ctx, line, log_level);
}

#define _KL_LOG_IMPL(LOG_LEVEL) \
if(format != NULL) { \
   char buffer[4096]; \
   va_list vl; \
   va_start(vl, format); \
   vsnprintf(buffer, sizeof(buffer), format, vl); \
   _kl_write_console_line(buffer, LOG_LEVEL); \
   va_end(vl); \
}

void kl_log_info(const char* format, ...)
{
   _KL_LOG_IMPL(KL_LOG_LEVEL_INFO);
}

void kl_log_debug(const char* format, ...)
{
   _KL_LOG_IMPL(KL_LOG_LEVEL_DEBUG);
}

void kl_log_warn(const char* format, ...)
{
   _KL_LOG_IMPL(KL_LOG_LEVEL_WARN);
}

void kl_log_err(const char* format, ...)
{
   _KL_LOG_IMPL(KL_LOG_LEVEL_ERR);
}

#undef _KL_LOG_IMPL
