-- -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

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

assert(false, "This file is used to generate documentation only, and should not be executed.")

--! @class RenderTarget
--! Interface to the render targets in the runtime.
--! @ingroup script_rendering
RenderTarget = {}

--! Create a new RenderTarget object.
--!
--! @param width  Width for the RenderTarget.
--! @param height Height for the RenderTarget.
--! @return The new RenderTarget object.
--! @memberof RenderTarget
function RenderTarget.new(width, height)
end

--! Update the width/height of an existing RenderTarget.
--!
--! @param width  New width of the RenderTarget.
--! @param height New height of the RenderTarget.
--! @memberof RenderTarget
function RenderTarget:insert(width, height)
end
