-- an implementation of printf

function printf(...)
   io.write(string.format(...))
end

printf("Hello %s from %s on %s\n",os.getenv"USER" or "there",_VERSION,os.date())

function main(...)
   print("FlightlessManicotti")

   for i = 1, arg["n"] do
      print("\t" .. arg[i])
   end

   -- Dump this module
   for key,value in pairs(scriptevent) do
      print(key..": "..tostring(value))
   end

   -- Try dequeue
   evt_id,foo,bar,baz = scriptevent.dequeue(SCTX)
   while evt_id do
      print("From code: "..evt_id..","..foo..","..bar..","..tostring(baz)) 
      evt_id,foo,bar,baz = scriptevent.dequeue(SCTX)
   end

   -- Try enqueue
   scriptevent.enqueue(SCTX, 1, 2, 3, 4)
   scriptevent.enqueue(SCTX, 5, 6, 7, 8)
end
