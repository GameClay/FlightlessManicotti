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

--! Allocate a new Scene2D.
--!
--! @param max_entities Maximum number of entities which can be stored in the scene.
--!
--! @return Newly allocated scene.
--! @memberof Scene2D
function Scene2D.new(max_entities)
end

--! Reserve an entity in the scene.
--!
--! @param reference The reference that will be stored and returned by raycasts, queries, etc.
--!
--! @return Reserved entity.
--! @memberof Scene2D
function Scene2D:reserve(reference)
end

--! Perform a raycast.
--!
--! @param from   A vector2d indicating the start of the ray.
--! @param to     A vector2d indicating the end of the ray.
--! @param type   A typemask to filter the allowed hits.
--!
--! @memberof Scene2D
function Scene2D:raycast(from, to, type)
end
