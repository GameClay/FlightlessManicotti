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

#ifndef _KL_LOG_H_
#define _KL_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>

/**
 * Console log callback function pointer.
 */
typedef void (*kl_console_line_callback_fn)(void* context, const char* line, int log_level);

/**
 * Assign a console log callback.
 *
 */
extern KL_API void kl_console_line_callback_assign(kl_console_line_callback_fn callback_fn, void* context);

/**
 * Log a line of text with the info tag.
 *
 * @param format Format string.
 */
extern KL_API void kl_log_info(const char* format, ...);

/**
 * Log a line of text with the debug tag.
 *
 * @param format Format string.
 */
extern KL_API void kl_log_debug(const char* format, ...);

/**
 * Log a line of text with the warning tag.
 *
 * @param format Format string.
 */
extern KL_API void kl_log_warn(const char* format, ...);

/**
 * Log a line of text with the error tag.
 *
 * @param format Format string.
 */
extern KL_API void kl_log_err(const char* format, ...);

/**
 * Log a line of text with the info tag.
 *
 * @param format Format string.
 */
#define kl_log kl_log_info

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
