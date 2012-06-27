/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* Sanskrit -- Copyright (C) 2011 GameClay LLC
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

#ifndef _SANSKRIT_H_
#define _SANSKRIT_H_

#if defined(SKLOG_ASL)
#  include <asl.h>
#elif defined(SKLOG_SYSLOG)
#  include <syslog.h>
#elif defined(SKLOG_ANDROID)
#  include <android/log.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/* Disable unused function warnings for GCC */
#if (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 6)
#  define _SK_UNUSED_FUNC_WARNING_DISABLED_
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif

#if 0
#  define SK_INLINE inline
#else
#  define SK_INLINE
#endif

/* Log levels */
#if defined(SKLOG_ASL)
#  define SKLOG_LEVEL_DEBUG   ASL_LEVEL_DEBUG
#  define SKLOG_LEVEL_INFO    ASL_LEVEL_INFO
#  define SKLOG_LEVEL_WARN    ASL_LEVEL_WARNING
#  define SKLOG_LEVEL_ERR     ASL_LEVEL_ERR
#elif defined(SKLOG_SYSLOG)
#  define SKLOG_LEVEL_DEBUG   LOG_DEBUG
#  define SKLOG_LEVEL_INFO    LOG_INFO
#  define SKLOG_LEVEL_WARN    LOG_WARNING
#  define SKLOG_LEVEL_ERR     LOG_ERR
#elif defined(SKLOG_ANDROID)
#  define SKLOG_LEVEL_DEBUG   ANDROID_LOG_DEBUG
#  define SKLOG_LEVEL_INFO    ANDROID_LOG_INFO
#  define SKLOG_LEVEL_WARN    ANDROID_LOG_WARN
#  define SKLOG_LEVEL_ERR     ANDROID_LOG_ERROR
#else
#  define SKLOG_LEVEL_DEBUG   0
#  define SKLOG_LEVEL_INFO    1
#  define SKLOG_LEVEL_WARN    3
#  define SKLOG_LEVEL_ERR     4
#endif

/* Redefine this to change the source-name of the log messages */
#ifndef SKLOG_IDENT
#  define SKLOG_IDENT Sanskrit
#endif
#define _SK_STRINGIFY(x) #x
#define SK_STRINGIFY(x) _SK_STRINGIFY(x)

#define _SKLOG_IMPL(LOG_LEVEL)                        \
   {                                                  \
      va_list vl;                                     \
      va_start(vl, format);                           \
      sklogv(LOG_LEVEL, format, vl);                  \
      va_end(vl);                                     \
   }

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Sanskrit.
 *
 * This function must be called before any other logging function to
 * initialize syslog or asl.
 *
 * @return 0 if successful, 1 otherwise.
 */
extern int sklog_init(void);

/**
 * Destroy Sanskrit.
 */
extern void sklog_destroy(void);

/**
 * Core logging function.
 *
 * All other log functions call this function for logging.
 *
 * @param log_level  One of: SKLOG_LEVEL_DEBUG, SKLOG_LEVEL_INFO, SKLOG_LEVEL_WARN, SKLOG_LEVEL_ERR
 * @param format     String format.
 * @param vargs      Args to go with format
 */
extern void sklogv(int log_level, const char* format, va_list vargs);

/* sklog == skinfo */
#define sklog skinfo

#ifndef SK_DISABLE_LOGS

/* Info */
#  ifndef SK_DISABLE_INFO
static SK_INLINE void skinfo(const char* format, ...)
{
   _SKLOG_IMPL(SKLOG_LEVEL_INFO);
}
#  else
static SK_INLINE void skinfo(const char* format, ...) {}
#  endif

/* Debug */
#  ifndef SK_DISABLE_DEBUG
static SK_INLINE void skdebug(const char* format, ...)
{
   _SKLOG_IMPL(SKLOG_LEVEL_DEBUG);
}
#  else
static SK_INLINE void skdebug(const char* format, ...) {}
#  endif

/* Warning */
#  ifndef SK_DISABLE_WARN
static SK_INLINE void skwarn(const char* format, ...)
{
   _SKLOG_IMPL(SKLOG_LEVEL_WARN);
}
#  else
static SK_INLINE void skwarn(const char* format, ...) {}
#  endif

/* Error */
#  ifndef SK_DISABLE_ERROR
static SK_INLINE void skerr(const char* format, ...)
{
   _SKLOG_IMPL(SKLOG_LEVEL_ERR);
}
#  else
static SK_INLINE void skerr(const char* format, ...) {}
#  endif

/* Prevent warnings/errors about unused functions */
#ifndef _SK_UNUSED_FUNC_WARNING_DISABLED_
static void _sk_disable_unused_never_call_this_2_();
static void _sk_disable_unused_never_call_this_1_()
{
   skinfo("");
   skdebug("");
   skwarn("");
   skerr("");
   _sk_disable_unused_never_call_this_2_();
}

static void _sk_disable_unused_never_call_this_2_()
{
   _sk_disable_unused_never_call_this_1_();
}
#endif

#else
/* Disable all the things */
static SK_INLINE void skinfo(const char* format, ...) {}
static SK_INLINE void skdebug(const char* format, ...) {}
static SK_INLINE void skwarn(const char* format, ...) {}
static SK_INLINE void skerr(const char* format, ...) {}
#endif

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#if (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 6)
#     pragma GCC diagnostic pop
#endif
