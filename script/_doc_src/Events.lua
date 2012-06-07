-- -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

-- FlightlessManicotti -- Copyright (C) 2010-2012 GameClay LLC
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

assert(false, "This file is used to generate documentation only, and should not be executed.")

--! @class Events
--! Interface to the event-queue from the runtime.
--! @ingroup script_events
Events = {}

--! Dequeue the next event.
--!
--! @param sctx   Script context from which to dequeue.
--!
--! @return The id, context, and argument of the dequeued event.
--! @memberof Events
function Events.dequeue(sctx)
end

--! Acknowledge an end-of-frame event.
--!
--! @param sctx   Script context on which to ack. the eof event.
--!
--! @see Events.eof
--! @memberof Events
function Events.framedone(sctx)
end

--! Register a script event.
--!
--! Registering a script event is the way to obtain an id for
--! that event.
--!
--! @param sctx   Script context on which to register the event.
--! @param name   The name of the event to register.
--!
--! @return The id of the registered event.
--! @memberof Events
function Events.reigster(sctx, name)
end
