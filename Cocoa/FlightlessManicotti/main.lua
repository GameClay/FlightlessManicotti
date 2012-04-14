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

   local testscene = Scene3D.new(1024)

   testscene = nil

   --lsys_verts = sierpinski(10)
   lsys_verts = dragoncurve(10)
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

function testinit()
   print("Init called!")
end

function testdestroy()
   print("Destroy called!")
end
