-- -*- Mode: lua; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=lua tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

-- FlightlessManicotti -- Copyright (C) 2010-2012 GameClay LLC
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

--! @class ComponentBucket
--! Add docs here!
--! @ingroup script_components
ComponentBucket = {_components = {}}

--! Constructor.
--! @memberof ComponentBucket
function ComponentBucket:new()
   o = o or {}
   setmetatable(o, self)
   self.__index = self

   return o
end

--! Get all Components aggrigated into this ComponentBucket.
--! @memberof ComponentBucket
function ComponentBucket:components()
   return self._components
end

--! Aggregates a Component onto this ComponentBucket.
--!
--! @param component The component instance to aggregate.
--! @param name The name with which to register the provided component.
--!
--! @return True if the component was aggregated, false if not.
--! @memberof ComponentBucket
function ComponentBucket:addcomponent(component, name)
   -- If a component is registered with the same name, assert
   assert(self._components[name] == nil, "Component with name '"..name.."' already aggregated into this ComponentBucket. (I already haz that in mah bukkit!)")
   
   -- If the component doesn't want to be aggregated with us, bail gracefully
   if not (component:register(self, name)) then
      return false
   end
   
   self._components[name] = component
   
   -- Tell all the components to reset
   for k,v in pairs(self._components) do
      v:reset()
   end
   
   return true
end

--! Remove a Component from this ComponentBucket.
--! @memberof ComponentBucket
function ComponentBucket:removecomponent(component)
   -- Find component
   local found = false
   for k,v in pairs(self._components) do
      if v == component then
         found = true
         k:unregister()
         self._components[k] = nil
         break
      end
   end
   
   -- Component not found, bail
   assert(found, "Component has not been aggregated into this ComponentBucket. (Nooo! That been takin from mah bukkit!)")
   
   -- Tell all the components to reset
   for k,v in pairs(self._components) do
      v:reset()
   end
end
