-- -*- Mode: lua; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=lua tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

-- FlightlessManicotti -- Copyright (C) 2009-2010 GameClay LLC
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

require("game.entity.IEntityComponent")

--! Implementation of the IEntityComponent interface.
EntityComponent = IEntityComponent -- Is this the proper way to do an "interface"?

--! EntityComponent constructor.
function EntityComponent:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   
   self._isregistered = false
   self._owner = nil
   self._name = nil
   
   return o
end

function EntityComponent:owner(new_owner)
   if new_owner then
      self._owner = new_owner
   end
   return self._owner
end

function EntityComponent:name()
   return self._name
end

function EntityComponent:isregistered()
   return self._isregistered
end

function EntityComponent:register(owner, name)
   assert(not self:isregistered(), "Trying to register an already-registered component!")
   self._name = name
   self:owner(owner)
   self:onadd()
   self._isregistered = true
end

function EntityComponent:unregister()
   assert(self:isregistered(), "Trying to unregister an unregistered component!")
   
   self._isregistered = false
   self:onremove()
   
   self._owner = nil
   self._name = nil
end

function EntityComponent:reset()
   self:onreset()
end

function EntityComponent:onadd()
end

function EntityComponent:onremove()
end

function EntityComponent:onreset()
end
