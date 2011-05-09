-- Set up script library location
package.path = './script/?.lua;./script/?/init.lua;'..package.path

function dumptable(table)
   print(tostring(table))
   for key,value in pairs(table) do
      print("\t"..key..": "..tostring(value))
   end
end

-- Bring in script library
require 'script'
require 'game.components'
require 'events'

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
   
   local test = Component.new()
   print("I made a component! Is it registered? "..tostring(test:isregistered()))
end
