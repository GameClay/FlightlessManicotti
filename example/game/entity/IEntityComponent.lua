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

--! The base-interface for all components.
--!
--! A component  is used to define specific pieces of functionality for
--! game entities. Several components can be added to a single entity to give
--! the entity complex behavior while keeping the different functionalities 
--! separate from each other.
--!
--! A full featured implementation of this interface is included (EntityComponent).
--! It should be adequate for almost every situation, and therefore, custom components
--! should derive from it rather than implementing this interface directly.
--! 
--! @see IEntity
--! @see EntityComponent
IEntityComponent = {}

--! Set the owner. 
--! 
--! @note This should only be set by the owning entity.
function IEntityComponent:owner(new_owner) assert(false) end

--! The name given to the component when it is added to an entity.
function IEntityComponent:name() assert(false) end

--! Check if the component is currently registered with an entity.
function IEntityComponent:isregistered() assert(false) end

--! Registers the component with an entity.
--!
--! @note This should only ever be called by an entity 
--!       from the addcomponent method.
--! 
--! @param owner The entity to register the component with.
--! @param name The name to assign to the component.
function IEntityComponent:register(owner, name) assert(false) end

--! Unregisters the component from an entity. 
--!
--! @note This should only ever be called by an entity class from 
--!       the removeromponent method.
function IEntityComponent:unregister() assert(false) end

--! Tell the component to re-acquire any references to its owner.
--!
--! This is called by an entity on all of its components any time a component
--! is added or removed. In this method, any references to properties on the
--! owner entity should be purged and re-looked up.
function IEntityComponent:reset() assert(false) end