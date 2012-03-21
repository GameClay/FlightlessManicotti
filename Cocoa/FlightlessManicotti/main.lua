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
require 'game.components.scene.Scene2DComponent'

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
   test:register(nil, "facepunch")
   
   test.entity.position.xy = {0,3}
   test.entity.radius = 3.5
   test.entity.typemask = 8
   hit,pos = testscene:raycast({-5, 0}, {5, 0}, 8)
   print(tostring(hit).." at "..tostring(pos))
   
   test:unregister()
   
   testscene = nil
end