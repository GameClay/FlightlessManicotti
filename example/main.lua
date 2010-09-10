-- Set up script library location
package.path = package.path..';./script/?.lua'

function dumptable(table)
   print(tostring(table))
   for key,value in pairs(table) do
      print("\t"..key..": "..tostring(value))
   end
end

-- Bring in script library
require 'script'

function main(...)
   --dumptable(script.events)
   
   -- Use enqueue to send the args back to code
   -- NOTE: I am not sure if Lua will be able to enqueue to the script event queue,
   --       as this could cause a bad. On the other hand, people can find unlimited
   --       ways to do dumb stuff, and one of the design philosophies is:
   ---      'empower the scripter' so I am leaning twords keeping it  in.
   for i = 2, arg["n"] do
      script.events.enqueue(SCTX, tostring(arg[i]), nil, 0, 1, 2)
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
end

function event_handler()
   -- dequeue everything from code
   name,context,a,b,c = script.events.dequeue(SCTX)
   while name do
      print("From code: {"..name..","..tostring(context)..","..a..","..b..","..c.."}") 
      name,context,a,b,c = script.events.dequeue(SCTX)
   end
end

script.events.handler = event_handler