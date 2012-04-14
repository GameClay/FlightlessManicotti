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
--! A component which provides advancetime and processtick functionality.
--!
--! The ProcessComponent registers itself with the script event loop and
--! subscribes to Events.advancetime and Events.processtick.
--! @extends Component
--! @ingroup script_components
ProcessComponent = Component:new()

--! Constructor.
--! @memberof ProcessComponent
function ProcessComponent:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self

   assert(not processtickfn or type(processtickfn) == "function", "'processtick' must be a function.")
   assert(not advancetimefn or type(advancetimefn) == "function", "'advancetime' must be a function.")

   return o
end

--! Subscribes to Events.processtick and Events.advancetime.
--!
--! @memberof ProcessComponent
function ProcessComponent:onadded()
   if self.processtick then
      Events.subscribe(self.processtick, Events.processtick)
   end

   if self.advancetime then
      Events.subscribe(self.advancetime, Events.advancetime)
   end
end

--! Unsubscribes from Events.processtick and Events.advancetime.
--! @memberof ProcessComponent
function ProcessComponent:onremoved()
   -- Unsubscribe
   if self.processtick then
      Events.unsubscribe(self.processtick, Events.processtick)
   end

   if self.advancetime then
      Events.unsubscribe(self.advancetime, Events.advancetime)
   end
end
