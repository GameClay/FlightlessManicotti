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

--! @addtogroup script
--! @{

--! @addtogroup script_events
--! @{

--! Subscribe to events.
--!
--! @param  o  Object that desires subscription.
function Events.subscribe(o)
   
end

--! Event handler.
function Events.handler()
   -- dequeue everything from code
   id,context,arg = Events.dequeue(SCTX)
   while (not id or not (id == Events.eof)) do
      if id then
         if id == Events.advancetime then
            arg = Cast.int_to_float(arg)
            
            -- Pass to script advance time list
         elseif id == Events.processtick then
            -- Pass to script process tick list
         else
            print("Event: {"..id..","..tostring(context)..","..arg.."}")
         end
      end
      
      id,context,arg = Events.dequeue(SCTX)
   end

   -- If we hit end-of-frame, acknowledge it
   if(id == Events.eof) then
      Events.framedone(SCTX, context)
   end
end
--! @}
--! @}
