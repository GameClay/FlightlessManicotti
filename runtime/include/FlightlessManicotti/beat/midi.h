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
 
#ifndef _KL_MIDI_H_
#define _KL_MIDI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <FlightlessManicotti/fm.h>

typedef struct _kl_midi_manager* kl_midi_manager_t;

/**
 * Allocate a midi manager.
 *
 * @param manager    Location to store allocated midi manager.
 *
 * @return KL_SUCCESS if successful.
 */
extern KL_API int kl_midi_manager_alloc(kl_midi_manager_t* manager);

/**
 * Free a midi manager.
 *
 * @param manager    Midi manager to free.
 */
extern KL_API void kl_midi_manager_free(kl_midi_manager_t* manager);

#ifdef __cplusplus
}
#endif

#endif
