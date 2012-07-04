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

#include "../include/sanskrit/sklog.h"

#if defined(SKLOG_ASL)
aslclient _sklog_asl_client = NULL;
#endif
int _sklog_enabled = 0;

int sklog_init(void)
{
#if defined(SKLOG_ASL)
   if(_sklog_asl_client != NULL)
   {
      asl_close(_sklog_asl_client);
   }

   _sklog_asl_client = asl_open(SK_STRINGIFY(SKLOG_IDENT), "com.apple.console", ASL_OPT_STDERR | ASL_OPT_NO_DELAY);
   _sklog_enabled = (_sklog_asl_client == NULL ? 0 : 1);
#elif defined(SKLOG_SYSLOG)
   _sklog_enabled = 1;
   closelog();
   openlog(SK_STRINGIFY(SKLOG_IDENT), LOG_PERROR | LOG_NDELAY | LOG_PID, LOG_USER);
#else
   _sklog_enabled = 1;
#endif

   return (_sklog_enabled == 1 ? 0 : -1);
}

void sklog_destroy(void)
{
   _sklog_enabled = 0;
#if defined(SKLOG_ASL)
   if(_sklog_asl_client != NULL)
   {
      asl_close(_sklog_asl_client);
      _sklog_asl_client = NULL;
   }
#elif defined(SKLOG_SYSLOG)
   closelog();
#endif
}

void sklogv(int log_level, const char* format, va_list vargs)
{
   if(_sklog_enabled)
   {
#if defined(SKLOG_ASL)
      aslmsg msg = asl_new(ASL_TYPE_MSG);
      asl_set(msg, ASL_KEY_FACILITY, "com.apple.console");
      asl_vlog(_sklog_asl_client, msg, log_level, format, vargs);
      asl_free(msg);
#elif defined(SKLOG_SYSLOG)
      vsyslog(log_level, format, vargs);
#elif defined(SKLOG_ANDROID)
      __android_log_vprint(log_level, SK_STRINGIFY(SKLOG_IDENT), format, vargs);
#else
      fprintf(stderr, "<" SK_STRINGIFY(SKLOG_IDENT) ">: ");
      vfprintf(stderr, format, vargs);
      fprintf(stderr, "\n");
#endif
   }
}
