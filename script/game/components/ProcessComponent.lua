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

require 'events'
require 'game.components.Component'

--! @class ProcessComponent
--! Process Component.
--!
--! A process component registers itself with the script event loop and
--! subscribes to Events.advancetime and Events.processtick.
--! @extends Component
--! @ingroup script_components
DeclareComponentType("ProcessComponent")

--! @fn ProcessComponent.new(processtickfn, advancetimefn)
--! Constructor.
--! @memberof ProcessComponent
function ProcessComponent.new(processtickfn, advancetimefn)
   local o = {}
   setmetatable(o, {__index = ProcessComponent})
   
   assert(not processtickfn or type(processtickfn) == "function", "First argument to ProcessCompoenent.new must be a function.")
   assert(not advancetimefn or type(advancetimefn) == "function", "Second argument to ProcessCompoenent.new must be a function.")
   
   o._processtick = processtickfn
   o._advancetime = advancetimefn
   
   return o
end

--! @fn ProcessComponent:onadded()
--! Subscribes to Events.processtick and Events.advancetime.
--! @memberof ProcessComponent
function ProcessComponent:onadded()
   if self._processtick then
      Events.subscribe(self._processtick, Events.processtick)
   end
   
   if self._advancetime then
      Events.subscribe(self._advancetime, Events.advancetime)
   end
end

--! @fn ProcessComponent:onremoved()
--! Unsubscribes from Events.processtick and Events.advancetime.
--! @memberof ProcessComponent
function ProcessComponent:onremoved()
   -- Unsubscribe
   if self._processtick then
      Events.unsubscribe(self._processtick, Events.processtick)
   end
   
   if self._advancetime then
      Events.unsubscribe(self._advancetime, Events.advancetime)
   end
end
