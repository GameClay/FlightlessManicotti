-- -*- Mode: lua; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=lua tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

-- FlightlessManicotti -- Copyright (C) 2010-2011 GameClay LLC
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

--! @addtogroup script_events
--! @{
   
--! @cond
Events.subscriptions = {}

-- Table of argument/context conditioners for specific events.
Events.conditioners = {
   [Events.advancetime] = function(arg, context)
      return Cast.int_to_float(arg),context
   end,
   default = function(arg, context)
      return arg,context
   end
}
--! @endcond

--! Subscribe to events.
--!
--! @param  fn       Event handler being registered.
--! @param  eventid  Event id for which the specified object desires updates.
function Events.subscribe(fn, eventid)
   if type(fn) == "function" then
      Events.subscriptions[eventid] = Events.subscriptions[eventid] or {}
      table.insert(Events.subscriptions[eventid], fn)
   else
      error("First argument to Events.subscribe should be a function.")
   end
end

--! Unsubscribe from events.
--!
--! @param  fn       Event handler to remove.
--! @param  eventid  Event id from which to unsubscribe.
--! @return True if successful.
function Events.unsubscribe(fn_or_pos, eventid)
   if type(fn) == "function" then
      local handlers = Events.subscriptions[eventid] or {}
      for i,v in ipairs(handlers) do
         if v == fn_or_pos then
            table.remove()
            return true
         end
      end
   else
      error("First argument to Events.unsubscribe should be a function.")
   end
   
   return false
end

--! Event handler.
function Events.handler()
   -- Dequeue all events from code
   local id,context,arg = Events.dequeue(SCTX)
   while (not id or not (id == Events.eof)) do
      -- Do any event-specific argument/context conditioning
      local conditioner = Events.conditioners[id] or Events.conditioners.default
      arg,context = conditioner(arg, context)
      
      -- Execute handlers
      local handlers = Events.subscriptions[id] or {}
      for _,v in ipairs(handlers) do
         v(arg, context)
      end
      
      -- Next event
      id,context,arg = Events.dequeue(SCTX)
   end

   -- If we hit end-of-frame, acknowledge it
   if id == Events.eof then
      Events.framedone(SCTX, context)
   end
end
--! @}
