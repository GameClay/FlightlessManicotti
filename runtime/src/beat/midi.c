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

#include <FlightlessManicotti/beat/midi.h>
#include <FlightlessManicotti/process/process.h>

#include <CoreMIDI/CoreMIDI.h>

struct _kl_midi_manager {
   uint64_t pid;

   MIDIClientRef midi_client;
   MIDIPortRef midi_out_port;
   MIDIPortRef midi_in_port;

   MIDIEndpointRef hax_ep;
};

kl_midi_manager_t g_midi_manager = NULL;

void _kl_midi_manager_advance_time(float dt, void* context);
void _kl_midi_notify(const MIDINotification *message, void *refCon);
void _kl_midi_read(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);

int kl_midi_manager_alloc(kl_midi_manager_t* manager)
{
   int ret = KL_ERROR;
   if(manager != NULL)
   {
      kl_midi_manager_t mgr = kl_heap_alloc(sizeof(struct _kl_midi_manager));
      if(mgr != NULL)
      {
         OSStatus s = MIDIClientCreate(CFSTR("EVL MIDI Client"), _kl_midi_notify, mgr, &mgr->midi_client);

         s = MIDIOutputPortCreate(mgr->midi_client, CFSTR("EVL MIDI Output Port"), &mgr->midi_out_port);

         s = MIDIInputPortCreate(mgr->midi_client, CFSTR("EVL MIDI Input Port"), _kl_midi_read,
            mgr, &mgr->midi_in_port);

         /* hax */
         {
            const ItemCount numberOfSources      = MIDIGetNumberOfSources();

            if(numberOfSources > 0)
            {
               mgr->hax_ep = MIDIGetSource(0);
               MIDIPortConnectSource(mgr->midi_in_port, mgr->hax_ep, NULL);
            }
            else mgr->hax_ep = 0;
         }

         mgr->pid = kl_reserve_process_id(KL_DEFAULT_PROCESS_MANAGER,
            NULL, &_kl_midi_manager_advance_time, mgr);

         *manager = mgr;
         ret = KL_SUCCESS;
      }
   }
   return ret;
}

void kl_midi_manager_free(kl_midi_manager_t* manager)
{
   if(manager != NULL && *manager != NULL)
   {
      kl_midi_manager_t mgr = *manager;

      kl_release_process_id(KL_DEFAULT_PROCESS_MANAGER, mgr->pid);

      MIDIPortDisconnectSource(mgr->midi_in_port, mgr->hax_ep);
      MIDIPortDispose(mgr->midi_out_port);
      MIDIPortDispose(mgr->midi_in_port);
      MIDIClientDispose(mgr->midi_client);

      kl_heap_free(mgr);
   }
}

void _kl_midi_manager_advance_time(float dt, void* context)
{

}

void _kl_midi_notify(const MIDINotification *message, void *refCon)
{
   switch(message->messageID)
   {
      case kMIDIMsgObjectAdded:
         
         break;
      case kMIDIMsgObjectRemoved:
         
         break;
      case kMIDIMsgSetupChanged:
      case kMIDIMsgPropertyChanged:
      case kMIDIMsgThruConnectionsChanged:
      case kMIDIMsgSerialPortOwnerChanged:
      case kMIDIMsgIOError:
         break;
   }
}

void _kl_midi_read(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon)
{
   int numPackets;
   const MIDIPacket *packet = &pktlist->packet[0];
   for(numPackets = pktlist->numPackets; numPackets > 0; numPackets--)
   {
      packet = MIDIPacketNext(packet);
   }
}
