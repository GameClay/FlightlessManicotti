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


--! Component which does stuff and things for a 2d scene.
Scene2DComponent = {}
setmetatable(Scene2DComponent, {__index = Component})

--! Constructor.
function Scene2DComponent.new()
   o = {}
   setmetatable(o, {__index = Scene2DComponent})
   return o
end

function Scene2DComponent:onadded()
   -- Register with the C code and reserve an id
   if self._scene then
      self._scene_id = self._scene:reserveid()
   else
      error("No scene assigned. Call 'assignscene' before registering component.")
   end
end

function Scene2DComponent:onremoved()
   -- Release our reserved id
   if self._scene_id then
      self._scene:releaseid(self._scene_id)
   end
end

function Scene2DComponent:assignscene(scene)
   self._scene = scene
end
