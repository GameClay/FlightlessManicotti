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

--! @class Lsystem
Lsystem = {_start = {}, _rules = {}}

--! Constructor.
--! @memberof Lsystem
function Lsystem:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self

   return o
end

function Lsystem:setstart(start)
   self._start = start
end

function Lsystem:addrule(key, action, weight)
   local rule = {}
   rule.action = action
   rule.weight = weight or 1
   self._rules[key] = self._rules[key] or {}
   table.insert(self._rules[key], rule)
end

function Lsystem.state_to_string(state)
   local ret = ""
   if state then
      for i = 1, #state do
         ret = ret..state[i]
      end
   end
   return ret
end

function Lsystem:getstate(iterations, randomseed)
   local state  = self._start

   --print(Lsystem.state_to_string(state))
   math.randomseed(randomseed or 42)
   for n = 1, iterations do
      local newstate = {}
      for i = 1, #state do
         local elem = state[i]

         local newelem = {elem}

         -- Select the correct next element, or keep old element
         if self._rules[elem] then
            -- Sum weights of all rules with the same key
            local sum_weight = 0
            for j = 1, #self._rules[elem] do
               sum_weight = self._rules[elem][j].weight + sum_weight
            end

            -- Pick a random rule
            local rnd = math.random(sum_weight)
            sum_weight = 1
            for j = 1, #self._rules[elem] do
               if rnd >= sum_weight and rnd < sum_weight + self._rules[elem][j].weight then
                  newelem = self._rules[elem][j].action
                  break
               end
               sum_weight = self._rules[elem][j].weight + sum_weight
            end
         end

         -- Append into newstate
         for j = 1, #newelem do
            table.insert(newstate, newelem[j])
         end
      end
      state = newstate
      --print(Lsystem.state_to_string(state))
   end

   return state
end
