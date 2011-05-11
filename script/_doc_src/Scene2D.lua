-- -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

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

assert(false, "This file is used to generate documentation only, and should not be executed.")

--! @defgroup script_scene2d 2D Scene
--! @ingroup script
--! Two-dimensional scene, and friends.

--! @class Scene2D
--! A 2D Scene container.
--! @ingroup script_scene2d
Scene2D = {}

--! Reserve an id in the scene.
--! @return Reserved scene id.
--! @memberof Scene2D
function Scene2D:reserveid()
end

--! Release a held id.
--!
--! @param id  Id to release.
--! @memberof Scene2D
function Scene2D:releaseid(id)
end
