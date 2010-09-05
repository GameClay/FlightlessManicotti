-- an implementation of printf

function dumptable(table)
   print(tostring(table))
   for key,value in pairs(table) do
      print("\t"..key..": "..tostring(value))
   end
end

io.write(string.format("[FlightlessManicotti] Hello %s from %s on %s\n",os.getenv"USER" or "there",_VERSION,os.date()))

function main(...)
   -- Dump the module to make sure we have all our stuff
   -- dumptable(scriptevent)

   -- dequeue everything from code
   name,context,a,b,c = scriptevent.dequeue(SCTX)
   while name do
      print("From code: {"..name..","..tostring(context)..","..a..","..b..","..c.."}") 
      name,context,a,b,c = scriptevent.dequeue(SCTX)
   end

   -- Use enqueue to send the args back to code
   for i = 2, arg["n"] do
      scriptevent.enqueue(SCTX, tostring(arg[i]), nil, 0, 1, 2)
   end
end
