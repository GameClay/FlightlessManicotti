-- -*- Mode: lua; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*-
-- vim: set filetype=lua tabstop=3 softtabstop=3 shiftwidth=3 expandtab:

-- FlightlessManicotti -- Copyright (C) 2010-2011 GameClay LLC
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

--! @addtogroup script
--! @{

--! @defgroup script_events Events
--! Script event handling.
--! @{

--! End-of-frame event.
Events.eof = Events.register(SCTX, "EOF")

--! AdvanceTime event.
--!
--! This event occurs once, and only once, each frame.
--!
--! The amount of time which has past since the previous
--! AdvanceTime event is provided as an argument to the event handler.
Events.advancetime = Events.register(SCTX, "AdvanceTime")

--! ProcessTick event.
--!
--! This event occurs once for each tick which has past. This means
--! that several ProcessTick events may occur in rapid succession if
--! the simulation needs to "catch up".
--!
--! There are no arguments passed to this event handler.
Events.processtick = Events.register(SCTX, "ProcessTick")

--@}
--@}

require 'events.Handler'