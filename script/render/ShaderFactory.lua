-- From: http://prideout.net/blog/?p=1
ApiVersionToLanguageVersion = { GL2 = 120, GL3 = 150, GL4 = 150 }

--! Define a shader for use by the rendering subsystem.
function DeclareShader(shaderKey, shaderSource)
   -- Prepend the line number directive for proper error messages.
   local lineNumber = debug.getinfo(2).currentline
   shaderSource = "#line " .. lineNumber .. "\n" .. shaderSource

   -- Extract the technique name from the fullpath of the Lua script.
   local fullpath = debug.getinfo(2).source
   local f, l, technique = string.find(fullpath, "([A-Za-z]+)%.lua")

   -- If a table for this technique does not exist, create it.
   if _G[technique] == nil then
      _G[technique] = {}
   end

   -- Make sure this shader hasn't already been declared.
   if _G[technique][shaderKey] then
      error("Shader '" .. shaderKey .. "' has been declared twice.")
   end

   -- Check if an API version is in the shader key and prepend #version.
   local pos = 0
   repeat
      dummy, pos, token = string.find(shaderKey, "([A-Za-z0-9]+)", pos + 1)
      if token and ApiVersionToLanguageVersion[token] then
         local langVersion = ApiVersionToLanguageVersion[token]
         shaderSource = "#version " .. langVersion .. "\n" .. shaderSource
      end
   until token == nil

   -- Add the shader to Lua's globals.
   _G[technique][shaderKey] = shaderSource
end
