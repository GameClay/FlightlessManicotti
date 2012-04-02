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
end

function testinit()
   print("Init called!")
end

function testdestroy()
   print("Destroy called!")
end
