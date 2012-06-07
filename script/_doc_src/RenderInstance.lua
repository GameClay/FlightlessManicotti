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

--! @class RenderInstance
--! Interface to the render list on the runtime.
--! @ingroup script_rendering
RenderInstance = {}

--! Create a new RenderInstance object.
--!
--! @return The new RenderInstance object.
--! @memberof RenderInstance
function RenderInstance.new()
end

--! Assign a Mesh to a RenderInstance.
--!
--! @param mesh Mesh to assign to this RenderInstance.
--! @memberof RenderInstance
function RenderInstance:setmesh(mesh)
end

--! Assign a material to a RenderInstance.
--!
--! @param name Name of the material to assign to this RenderInstance.
--! @memberof RenderInstance
function RenderInstance:setmaterial(name)
end

--! Assign an object-to-world transform to a RenderInstance.
--!
--! NOT YET IMPLEMENTED.
--! @memberof RenderInstance
function RenderInstance:settransform()
end

--! Set the draw type of a RenderInstance.
--!
--! @see RenderInstance.drawtype
--!
--! @param draw_type Draw type to use for rendering the Mesh assigned to this instance.
--! @memberof RenderInstance
function RenderInstance:setdrawtype(draw_type)
end


--! Set the blend parameters for a RenderInstance.
--!
--! @see RenderInstance.blend
--!
--! @param src_blend Source blend type.
--! @param dst_blend Destination blend type.
--! @memberof RenderInstance
function RenderInstance:setblend(src_blend, dst_blend)
end

--! Set the shader constants for a RenderInstance.
--!
--! @param shaderconsts Table of shader constants as name-value pairs {constName1 = 2, constName2 = {0, 1, 2}, ...}
--! @memberof RenderInstance
function RenderInstance:setshaderconstants(shaderconsts)
end

--! Update a shader constant value for a RenderInstance.
--!
--! @param shaderconsts Name-value pairs to update {constName1 = 42, constName2 = {3, 4, 5}, ...}
--! @memberof RenderInstance
function RenderInstance:updateshaderconstants(shaderconsts)
end

--! Assign a RenderTarget for a RenderInstance.
--!
--! Pass 'nil' to set back to the default target.
--!
--! @param target RenderTarget to draw this RenderInstance to.
--! @memberof RenderInstance
function RenderInstance:setrendertarget(target)
end

--! Specify if the assigned render target should be cleared before drawing this instance.
--!
--! @param clear Boolean value specifying if the assigned target should be cleared before drawing this instance.
--! @memberof RenderInstance
function RenderInstance:setclearbeforedraw(clear)
end
