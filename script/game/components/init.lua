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

--! @defgroup script_components Components
--! Components.
--!
--! A Component encapsulates a specific piece of functionality for
--! a game. To create complex functionality, multiple components
--! are aggregated together, as opposed to creating large inheretable 
--! object structures.
--!
--! @ingroup script

--! @fn DeclareComponentType(parent)
--! Declare a new component type.
--!
--! This helper function sets up the metatable and provides an
--! entry point for any other helper code that may be needed.
--!
--! @param  parent  Optional Component type to inheret from. If nil, Component is used as the parent type.
--!
--! @relates Component
--! @ingroup script_components
function DeclareComponentType(parent)
   assert(not parent or type(parent) == "table", "First parameter of DeclareComponentType should be nil, or a table.")
   
   local r = {}
   setmetatable(r, {__index = parent or Component})
   return r
end

require 'game.components.Component'
require 'game.components.ComponentBucket'
