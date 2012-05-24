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

#include <FlightlessManicotti/beat/midi.h>
#include <FlightlessManicotti/process/process.h>

#include <CoreMIDI/CoreMIDI.h>

struct _kl_midi_manager {
   uint64_t pid;

   MIDIClientRef midi_client;
   MIDIPortRef midi_out_port;
   MIDIPortRef midi_in_port;

   MIDIEndpointRef hax_in_ep;
   MIDIEndpointRef hax_out_ctrl_ep;
   kl_midi_device_t hax_device;
};

kl_midi_manager_t g_midi_manager = NULL;

void _kl_midi_manager_advance_time(float dt, void* context);
void _kl_midi_notify(const MIDINotification *message, void *refCon);
void _kl_midi_read(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);

void TestMIDICompletionProc(MIDISysexSendRequest *request)
{
   printf("%s\n", &request->data[8]);
}

int kl_midi_manager_alloc(kl_midi_manager_t* manager)
{
   int ret = KL_ERROR;
   if(manager != NULL)
   {
      kl_midi_manager_t mgr = kl_heap_alloc(sizeof(struct _kl_midi_manager));
      if(mgr != NULL)
      {
         CFStringRef tempstr = CFStringCreateWithCString(NULL, "EVL MIDI Client", kCFStringEncodingUTF8);
         OSStatus s = MIDIClientCreate(tempstr, _kl_midi_notify, mgr, &mgr->midi_client);
         CFRelease(tempstr);

         tempstr = CFStringCreateWithCString(NULL, "EVL MIDI Output Port", kCFStringEncodingUTF8);
         s = MIDIOutputPortCreate(mgr->midi_client, tempstr, &mgr->midi_out_port);
         CFRelease(tempstr);

         tempstr = CFStringCreateWithCString(NULL, "EVL MIDI Input Port", kCFStringEncodingUTF8);
         s = MIDIInputPortCreate(mgr->midi_client, tempstr, _kl_midi_read,
            mgr, &mgr->midi_in_port);
         CFRelease(tempstr);

         /* hax */
         kl_zero_mem(&mgr->hax_device, sizeof(kl_midi_device_t));
         {
            MIDIDeviceRef deviceref;
            int i;
            const ItemCount numberOfSources      = MIDIGetNumberOfSources();
            const ItemCount numberOfDestinations = MIDIGetNumberOfSources();

            deviceref = MIDIGetDevice(0);

            mgr->hax_in_ep = 0;
            mgr->hax_out_ctrl_ep = 0;

            if(numberOfSources > 0)
            {
               mgr->hax_in_ep = MIDIGetSource(0);
               MIDIPortConnectSource(mgr->midi_in_port, mgr->hax_in_ep, &mgr->hax_device);
            }

            for(i = 0; i < numberOfDestinations; i++)
            {
               CFStringRef name;
               MIDIObjectGetStringProperty(deviceref, kMIDIPropertyName, &name);
            }

            mgr->hax_out_ctrl_ep = MIDIGetDestination(2);
            MIDIPortConnectSource(mgr->midi_out_port, mgr->hax_out_ctrl_ep, &mgr->hax_device);
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

      MIDIPortDisconnectSource(mgr->midi_in_port, mgr->hax_in_ep);
      MIDIPortDisconnectSource(mgr->midi_in_port, mgr->hax_out_ctrl_ep);
      MIDIPortDispose(mgr->midi_out_port);
      MIDIPortDispose(mgr->midi_in_port);
      MIDIClientDispose(mgr->midi_client);

      kl_heap_free(mgr);
   }
}

void _kl_midi_manager_advance_time(float dt, void* context)
{
   /* moar hax */
   kl_midi_manager_t mgr = (kl_midi_manager_t)context;
   static int foo = 0;
   if(foo == 0)
   {
      static Byte data[] = {
          0xF0, 
          0x00, 0x20, 0x32, /* Berhinger */
          0x7F, /* Any device id */
          0x7F, /* Any model id */
          0x43, /* Request snapshot */
          /* No data */
          0xF7, /* End sysex */
      };

      static MIDISysexSendRequest sysex;

      sysex.destination = mgr->hax_out_ctrl_ep;
      sysex.data = data;
      sysex.bytesToSend = sizeof(data);
      sysex.complete = 0;
      sysex.completionProc = TestMIDICompletionProc;
      sysex.completionRefCon = NULL;

      /*MIDISendSysex(&sysex);*/
      foo = 1;
   }
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
   int i;
   const MIDIPacket *packet = &pktlist->packet[0];
   kl_midi_manager_t mgr = (kl_midi_manager_t)readProcRefCon;
   kl_midi_device_t* device = (kl_midi_device_t*)srcConnRefCon;
   kl_midi_device_t new_state;

   KL_UNUSED(mgr);

   kl_zero_mem(&new_state, sizeof(kl_midi_device_t));

   /* Read packets */
   for(i = pktlist->numPackets; i > 0; i--)
   {
      switch(packet->data[0])
      {
         case 176: /* Chan 1 Control/Mode Change */
         {
            const int8_t idx = packet->data[1];
            const int8_t value = packet->data[2];

            new_state.last_value[idx] = new_state.value[idx];
            device->last_sample_time[idx] = new_state.sample_time[idx];
            new_state.value[idx] = value;
            new_state.sample_time[idx] = packet->timeStamp;
            break;
         }
      }
      packet = MIDIPacketNext(packet);
   }

   /* Update device state */
   for(i = 0; i < 256; i++)
   {
      if(new_state.sample_time[i] > 0)
      {
         device->last_value[i] = device->value[i];
         device->last_sample_time[i] = device->sample_time[i];
         device->value[i] = new_state.value[i];
         device->sample_time[i] = new_state.sample_time[i];
         device->delta[i] = new_state.value[i] - device->last_value[i];
      }
   }

   /* Dispatch */
}
