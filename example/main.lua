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
require 'game.scene.Scene2DComponent'

-- Main is executed only once, it is not a loop. It receives the arguments
-- that were passed to the 'kl_initialize' function.
function main(...)
   
   -- Echo the arguments that the executable received
   for i = 2, arg["n"] do
      print("   "..tostring(arg[i]))
   end

   -- simple lsqlite3 test
   local db = sqlite3.open_memory()

   db:exec[[
     CREATE TABLE test (id INTEGER PRIMARY KEY, content);

     INSERT INTO test VALUES (NULL, 'Hello World');
     INSERT INTO test VALUES (NULL, 'Hello Lua');
     INSERT INTO test VALUES (NULL, 'Hello Sqlite3')
   ]]

   for row in db:nrows("SELECT * FROM test") do
     print(row.id, row.content)
   end
   
   local testscene = Scene2D.new(1024)
   
   local test = Scene2DComponent.new(testscene)
   local test2 = Scene2DComponent.new(testscene)
   
   test:register(nil, "facepunch")
   test2:register(nil, "facepunch2")
   
   print("Oh look it's "..tostring(test))
   print("Position: "..tostring(test.entity.position))
   print("Typemask: "..tostring(test.entity.typemask))
   print("Radius: "..tostring(test.entity.radius))
   print("Changing stuff...")
   test.entity.position.xy = {4,2}
   test.entity.radius = 3.5
   test.entity.typemask = 8
   print("Position: "..tostring(test.entity.position))
   print("Typemask: "..tostring(test.entity.typemask))
   print("Radius: "..tostring(test.entity.radius))
   
   test2.entity.position.xy = {5, 5}
   print("Position: "..tostring(test2.entity.position))
   test.entity.position = test2.entity.position
   print("Position: "..tostring(test.entity.position))
   
   test:unregister()
   test2:unregister()
   
   testscene = nil
end
