-- Set up script library location
package.path = './script/?.lua;./script/?/init.lua;'..package.path

function dumptable(table)
   print(tostring(table))
   if table then
      for key,value in pairs(table) do
         print("\t"..key..": "..tostring(value))
      end
   end
end

require 'events'
require 'game.components'
require 'game.components.scene.Scene3DComponent'
require 'game.Lsystem'
require 'parameters'
require 'parameters.spectrum.SpectrumComponent'

-- Main is executed only once, it is not a loop. It receives the arguments
-- that were passed to the 'kl_initialize' function.
function main(...)

   -- Echo the arguments that the executable received
   for i = 2, arg["n"] do
      print("   "..tostring(arg[i]))
   end

   -- Sign up for the Init/Destroy events
   Events.subscribe(testinit, Events.init)
   Events.subscribe(testdestroy, Events.destroy)
   Events.subscribe(testrenderinit, Events.renderinit)

   -- Test
   kick = Parameter:new("kick")
   kick:addcomponent(
      SpectrumComponent:new{
         onspectrumupdate = function (self, spectrum_sz, spectrum)
            local v = 0.0
            for i = 2, 5 do
               local f = spectrum[i]
               v = v + f * f
            end
            local val = math.sqrt(v / 3)
            self:owner():setvalue(val)
         end
      },
   "spectrum")
end

function dragoncurve(iterations)
   local lsys = Lsystem:new()
   lsys:setstart({"F", "X"})
   lsys:addrule("X", {"X", "+", "Y", "F"})
   lsys:addrule("Y", {"F", "X", "-", "Y"})
   local lsys_state = lsys:getstate(iterations)

   local lsys_verts = {}
   local cx = -0.5
   local cy = -0.25
   local ctheta = 1.57079633
   local fwd = 0.035
   local dtheta = 1.57079633

   table.insert(lsys_verts, cx)
   table.insert(lsys_verts, cy)

   for i = 1, #lsys_state do
      if lsys_state[i] == "F" then
         cx = cx + fwd * math.cos(ctheta)
         cy = cy + fwd * math.sin(ctheta)

         table.insert(lsys_verts, cx)
         table.insert(lsys_verts, cy)
      elseif lsys_state[i] == "+" then
         ctheta = ctheta - dtheta
      elseif lsys_state[i] == "-" then
         ctheta = ctheta + dtheta
      end
   end

   return lsys_verts
end

function sierpinski(iterations)
   local lsys = Lsystem:new()
   lsys:setstart({"A"})
   lsys:addrule("A", {"B", "-", "A", "-", "B"})
   lsys:addrule("B", {"A", "+", "B", "+", "A"})
   local lsys_state = lsys:getstate(iterations)

   local lsys_verts = {}
   local cx = -0.95
   local cy = -0.95
   local ctheta = 0
   local fwd = 0.0018
   local dtheta = 1.04719755

   table.insert(lsys_verts, cx)
   table.insert(lsys_verts, cy)

   for i = 1, #lsys_state do
      if lsys_state[i] == "A" or lsys_state[i] == "B" then
         cx = cx + fwd * math.cos(ctheta)
         cy = cy + fwd * math.sin(ctheta)

         table.insert(lsys_verts, cx)
         table.insert(lsys_verts, cy)
      elseif lsys_state[i] == "+" then
         ctheta = ctheta + dtheta
      elseif lsys_state[i] == "-" then
         ctheta = ctheta - dtheta
      end
   end

   return lsys_verts
end

function surface(slices, stacks, fn)
   local verts = {}
   for i = 0, slices do
      local theta = i * math.pi / slices
      for j = 0, (stacks - 1) do
         local phi = j * 2 * math.pi / stacks
         local vert = fn(theta, phi)
         table.insert(verts, vert)
      end
   end

   local faces = {}
   local v = 0
   for i = 0, (slices - 1) do
      for j = 0, (stacks - 1) do
         local n = (j + 1) % stacks

         local a = v + n
         local b = v + j
         local c = v + j + stacks
         local d = v + n + stacks

         table.insert(faces, {a, b, c})
         table.insert(faces, {a, c, d})
      end
      v = v + stacks
   end

   local mesh = Mesh.new()
   mesh:setpositions(verts)
   mesh:setfaces(faces)
   mesh:computenormals()
   mesh:update(Mesh.element.vertex + Mesh.element.normal + Mesh.element.index, Mesh.element.none)
   return mesh
end

function sphere(u, v)
   local x =  math.sin(u) * math.cos(v)
   local y =  math.cos(u)
   local z = -math.sin(u) * math.sin(v)
   return {x, y, z}
end

function testrenderinit()
   print("RenderInit called!")

   -- Moar test
   local testmesh = surface(15, 15, sphere)

   --[[local testmesh = Mesh.new()
   testmesh:loadctm("./bunny.ctm")
   testmesh:computenormals()
   testmesh:update(Mesh.element.vertex + Mesh.element.normal + Mesh.element.index, Mesh.element.none)]]

   testmesh:setashaxmesh()
end

function testinit()
   print("Init called!")
end

function testdestroy()
   print("Destroy called!")
end
