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

--! @class Mesh
--! Interface to the runtime mesh library.
--! @ingroup script_rendering
Mesh = {}

--! Create a new Mesh object.
--!
--! @return The new Mesh object.
--! @memberof Mesh
function Mesh.new()
end

--! Update the GPU data of a Mesh object.
--!
--! @param update_mask   Elements of the Mesh to update.
--! @param dynamic_mask  Elements of the Mesh which are dynamic buffers.
--! @memberof Mesh
function Mesh:update(update_mask, dynamic_mask)
end

--! Automatically compute normals for a mesh.
--!
--! Winding order should be clockwise.
--! @memberof Mesh
function Mesh:computenormals()
end

--! Load a CTM mesh.
--!
--! @param ctm_name The filename of the CTM mesh to load.
--! @memberof Mesh
function Mesh:loadctm(ctm_name)
end

--! Get the vertex positions of a Mesh.
--!
--! @return The vertex positions of the Mesh as a mutable vector3d_array.
--! @memberof Mesh
function Mesh:getpositions()
end

--! Set the vertex positions of a Mesh.
--!
--! @param positions Vertex positions for the Mesh as a table of vertices {{v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z}, ...}
--! @memberof Mesh
function Mesh:setpositions(positions)
end

--! Get the texture coordinates of a Mesh.
--!
--! @return The texture coordinates of the Mesh as a mutable vector2d_array.
--! @memberof Mesh
function Mesh:gettexcoords()
end

--! Set the texture coordinates of a Mesh.
--!
--! @param texcoords Texture coordinates for the mesh as a table of texture coords {{v1.u, v1.v}, {v2.u, v2.v}, ...}
--! @memberof Mesh
function Mesh:settexcoords(texcoords)
end

--! Gets the indices for the Mesh.
--!
--! @return The indices for the Mesh as a mutable uint16_array.
--! @memberof Mesh
function Mesh:getindices()
end

--! Set the indices for the Mesh.
--!
--! @param indices The indices for the mesh as a table of indices {i1, i2, i3, i4, ...}
--! @memberof Mesh
function Mesh:setindices(indices)
end

--! Set the indices of the Mesh as faces.
--!
--! @param faces Faces of the mesh as a table of faces {{f11, f12, f13}, {f21, f22, f23}, ...}
--! @memberof Mesh
function Mesh:setfaces(faces)
end
