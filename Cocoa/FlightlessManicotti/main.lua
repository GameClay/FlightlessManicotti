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

   lsys_verts = sierpinski(10)
   --lsys_verts = dragoncurve(10)
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

function spheremesh(M, N)
   local mesh = Mesh.new()
   mesh:reserve(M * N, 0)

   local pos = mesh:getpositions()
   for m = 0, (M - 1) do
      for n = 0, (N - 1) do
         local idx = n + m * N + 1
         pos[idx].x =  math.sin(math.pi * m / M) * math.cos(2 * math.pi * n / N)
         pos[idx].y =  math.cos(math.pi * m / M)
         pos[idx].z = -math.sin(math.pi * m / M) * math.sin(2 * math.pi * n / N)
      end
   end

   mesh:update(Mesh.element.vertex, Mesh.element.none)
   return mesh
end

function testrenderinit()
   print("RenderInit called!")

   -- Moar test
   testmesh = spheremesh(20, 20)
   testmesh:update(Mesh.element.vertex + Mesh.element.index, Mesh.element.none)
   testmesh:setashaxmesh()

   --testmesh = Mesh.new()
   --[[testmesh:reserve(4, 6)
   local pos = testmesh:getpositions()
   pos[1] = -0.5
   pos[2] = -0.5
   pos[3] =  0.0

   pos[4] = -0.5
   pos[5] =  0.5
   pos[6] =  0.0

   pos[7] =  0.5
   pos[8] = -0.5
   pos[9] =  0.0

   pos[10] = 0.5
   pos[11] = 0.5
   pos[12] = 0.0

   local idx = testmesh:getindices()
   idx[1] = 0
   idx[2] = 1
   idx[3] = 2
   idx[4] = 2
   idx[5] = 1
   idx[6] = 3]]

   --testmesh:loadctm("./bunny.ctm")
   --testmesh:computenormals()
   --testmesh:update(Mesh.element.normal, Mesh.element.none)
   --testmesh:update(Mesh.element.vertex + Mesh.element.normal + Mesh.element.index, Mesh.element.none)

   --testmesh:setashaxmesh()
end

function testinit()
   print("Init called!")
end

function testdestroy()
   print("Destroy called!")
end
