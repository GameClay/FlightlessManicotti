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

require 'game.components.Component'

--! @class Scene2DComponent
--! A Component which allows an aggregated object to exist in a 2D scene.
--!
--! @extends Component
--! @ingroup script_components
--! @ingroup script_scene2d
Scene2DComponent = DeclareComponentType()

--! Constructor.
--!
--! @param  scene Scene with which this component should register during onadded.
--! @memberof Scene2DComponent
function Scene2DComponent.new(scene)
   local o = {}
   setmetatable(o, {__index = Scene2DComponent})
   
   assert(scene and (type(scene) == "userdata"), "First parameter to Scene2DComponent.new should be a Scene2D.")
   o.scene = scene
   
   return o
end

--! Registers with the 2D scene provided upon construction.
--!
--! @memberof Scene2DComponent
function Scene2DComponent:onadded()
   -- Register with the C code and reserve an entity
   if self.scene then
      self.entity = self.scene:reserve(self)
   else
      error("No scene assigned.")
   end
end

--! Removes aggregated object from the 2D scene.
--! @memberof Scene2DComponent
function Scene2DComponent:onremoved()
   -- Release our reserved entity (the __gc metamethod will do this)
   self.entity = nil
end
