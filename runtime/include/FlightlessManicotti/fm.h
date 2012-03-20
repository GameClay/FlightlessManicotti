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
 
#ifndef _KL_TOPLEVEL_H_
#define _KL_TOPLEVEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Do dllimport/dllexport if needed
 */
#if _MSC_VER
#  ifdef KL_BUILD_LIBRARY
#     define KL_API __declspec(dllexport)
#  else
#     define KL_API __declspec(dllimport)
#  endif
#else
#  define KL_API 
#endif

/*
 * Define KL_INLINE as static
 */
#define KL_INLINE static

/*
 * Define KL_RESTRICT
 */
#if __STDC_VERSION__ >= 199901L
#  define KL_RESTRICT restrict
#elif _MSC_VER
#  define KL_RESTRICT __restrict
#else
#  define KL_RESTRICT 
#endif

/*
 * Define KL_DEBUG
 */

/* Sanity check what is defined */
#if defined(DEBUG) || defined(_DEBUG)
#  if defined(NDEBUG)
#     error "Mismatch in debug flags."
#  endif
#elif defined(NDEBUG)
#  if defined(DEBUG) || defined(_DEBUG)
#     error "Mismatch in debug flags."
#  endif
#endif

#if (defined(DEBUG) || defined(_DEBUG)) && !defined(KL_DEBUG)
#  define KL_DEBUG 1
#else
#  undef KL_DEBUG
#endif

/*
 * Define KL_BOOL
 */
#define KL_BOOL int
#define KL_TRUE 1
#define KL_FALSE 0

/*
 * Define KL_ASSERT
 */
#if defined(KL_DEBUG) || defined(KL_ENABLE_ASSERTS)
#  define KL_ENABLE_ASSERTS
#  include <assert.h>
#endif
#define KL_ASSERT(x, msg) assert(x && msg)

/*
 * Define KL_UNUSED
 */
#define KL_UNUSED(x) (void)x

/* TODO: Include all the things? */
#include <FlightlessManicotti/core/memory.h>
#include <FlightlessManicotti/core/error_codes.h>
#include <FlightlessManicotti/core/logger.h>

/**
 * Initialize the runtime.
 *
 * @param use_threads  Run scripting on its own thread.
 * @param main_script  The startup script.
 * @param argc         Number of arguments.
 * @param argv         Array of arguments.
 * @return KL_SUCCESS if successful.
 *         KL_ERROR if unsuccessful.
 */
extern KL_API int kl_initialize(KL_BOOL use_threads, const char* main_script, int argc, const char* argv[]);

/**
 * Execute a single iteration of the "main loop"
 */
extern KL_API int kl_mainloop_iteration();

/**
 * Do cleanup before the runtime destruction occurs.
 *
 * @note The scripts should be given at least one more iteration before
 *       kl_destroy is called.
 */
extern KL_API void kl_predestroy();

/**
 * Destroy the runtime.
 */
extern KL_API void kl_destroy();

#ifdef __cplusplus
}
#endif

#endif
