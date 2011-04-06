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


--! Base class for components.
--!
--! A Component encapsulates a specific piece of functionality for
--! a Entity. To create complex functionality, multiple components
--! are aggregated together, as opposed to creating large inheretable 
--! object structures.
--!
--! @see Entity
Component = {}

--! Constructor.
function Component:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   
   self._isregistered = false
   self._owner = nil
   self._name = nil
   
   return o
end

--! Get the registered owner of this Component.
function Component:owner()
   return self._owner
end

--! Get the name of this Component assigned during registration.
function Component:name()
   return self._name
end

--! Returns true if this Component has been registered.
function Component:isregistered()
   return self._isregistered
end

--! Register a Component with an object.
--!
--! @param owner The Entity with which to register this Component.
--! @param name The name to assign to the Component.
--!
--! @return True if the Entity registering this Component should 
--!         continue with the registration, otherwise registration will fail.
function Component:register(owner, name)
   assert(not self:isregistered(), "Trying to register an already-registered component!")
   
   self._name = name
   self._owner = owner
   
   self:onadded()
   self._isregistered = true
   
   return true
end

--! Unregisters a Component.
function Component:unregister()
   assert(self:isregistered(), "Trying to unregister an unregistered component!")
   
   self._isregistered = false
   self:onremoved()
   
   self._owner = nil
   self._name = nil
end

--! Tell the Component to re-acquire any references to its owner or other Components.
--!
--! This function is called by a Entity on all of its Components
--! whenever a Component is added or removed.
function Component:reset()
   self:onreset()
end

--! Called when a Component is registered.
--!
--! The owner, and name of the Component have been assigned at invokation
--! however the Component is not yet registered.
--!
--! @see register
function Component:onadded()
end

--! Called when a Component is unregistered.
--!
--! The owner, and name of the Component are still valid at invocation
--! however the Component is no longer registered.
--!
--! @see unregister
function Component:onremoved()
end

--! Called when a Component is reset.
--!
--! @see Reset
function Component:onreset()
end
