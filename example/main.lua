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
end

-- Test registration of an event
eof_evt_id = script.events.register(SCTX,"EOF")
advance_time_id = script.events.register(SCTX,"AdvanceTime")
process_tick_id = script.events.register(SCTX,"ProcessTick")

-- Define and assign event-handler.
function event_handler()
   -- dequeue everything from code
   id,context,arg = script.events.dequeue(SCTX)
   while (not id or not (id == eof_evt_id)) do
      if id then
         if id == advance_time_id then
            arg = cast.int_to_float(arg)
            
            -- Pass to script advance time list
         elseif id == process_tick_id then
            -- Pass to script process tick list
         else
            print("From code: {"..id..","..tostring(context)..","..arg.."}")
         end
      end
      
      id,context,arg = script.events.dequeue(SCTX)
   end

   -- If we just hit EOF, acknowledge the EOF
   if(id == eof_evt_id) then
      script.events.framedone(SCTX, context)
   end
end
script.events.handler = event_handler