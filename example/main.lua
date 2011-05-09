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

-- Test registration of an event
eof_evt_id = Events.register(SCTX, "EOF")
advance_time_id = Events.register(SCTX, "AdvanceTime")
process_tick_id = Events.register(SCTX, "ProcessTick")

-- Define and assign event-handler.
function event_handler()
   -- dequeue everything from code
   id,context,arg = Events.dequeue(SCTX)
   while (not id or not (id == eof_evt_id)) do
      if id then
         if id == advance_time_id then
            arg = Cast.int_to_float(arg)
            
            -- Pass to script advance time list
         elseif id == process_tick_id then
            -- Pass to script process tick list
         else
            print("From code: {"..id..","..tostring(context)..","..arg.."}")
         end
      end
      
      id,context,arg = Events.dequeue(SCTX)
   end

   -- If we just hit EOF, acknowledge the EOF
   if(id == eof_evt_id) then
      Events.framedone(SCTX, context)
   end
end
Events.handler = event_handler
