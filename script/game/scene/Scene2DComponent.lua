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
--! @param  scene Optional scene with which this component should register during onadded.
--! @memberof Scene2DComponent
function Scene2DComponent.new(scene)
   local o = {}
   setmetatable(o, {__index = Scene2DComponent})
   
   assert(not scene or (type(scene) == "userdata"), "First parameter to Scene2DComponent.new should be nil, or a Scene.")
   
   if scene then
      o._scene = scene
   end
   
   return o
end

--! Registers with the 2D scene provided.
--!
--! @see Scene2DComponent_assignscene
--! @memberof Scene2DComponent
function Scene2DComponent:onadded()
   -- Register with the C code and reserve an entity
   if self._scene then
      self._scene_entity = self._scene:reserve()
   else
      error("No scene assigned. Call 'assignscene' before registering component.")
   end
end

--! Removes aggregated object from the 2D scene.
--! @memberof Scene2DComponent
function Scene2DComponent:onremoved()
   -- Release our reserved entity
   self._scene_entity = nil
end

--! Assign a scene.
--!
--! Assigns a 2D scene to this component.
--!
--! @note Unless a scene was provided during construction, this must be called 
--!       before this component is aggregated, as the value provided is used in onadded().
--! @memberof Scene2DComponent
function Scene2DComponent:assignscene(scene)
   self._scene = scene
end

--! Get the position in the scene.
--!
--! @return vector2d position in the scene.
--! @memberof Scene2DComponent
function Scene2DComponent:position()
   assert(self._scene_entity, "Must have an assigned scene id to call position().")
   return self._scene_entity.position
end

--! Query the typemask of this object.
--!
--! @param type   [optional] Type to check against.
--!
--! @return True or false, if type is specified. The full typemask if not.
--! @memberof Scene2DComponent
function Scene2DComponent:type(typemask)
   assert(self._scene_entity, "Must have an assigned scene id to call type().")
   return self._scene_entity.type
end