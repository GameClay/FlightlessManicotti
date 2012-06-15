-- Adapted from: http://prideout.net/blog/?p=1
ShaderManager = {
   shaders = {},
   shader_paths = {},
   effects = {},
   ApiVersionToLanguageVersion = { GL2 = 120, GL3 = 150, GL4 = 150 },
   StringToShaderType = {
      Vertex = Shader.type.vertex,
      Fragment = Shader.type.fragment,
      Pixel = Shader.type.fragment,
      Geometry = Shader.type.geometry
   }
}

--! Define a shader for use by the rendering subsystem.
function ShaderManager.declareshader(shaderKey, shaderSource)
   -- Prepend the line number directive for proper error messages.
   local lineNumber = debug.getinfo(2).currentline
   shaderSource = "#line " .. lineNumber .. "\n" .. shaderSource

   -- Extract the technique name from the fullpath of the Lua script.
   local fullpath = debug.getinfo(2).source
   local f, l, technique = string.find(fullpath, "([A-Za-z]+)%.lua")

   -- If a table for this technique does not exist, create it.
   ShaderManager.shaders[technique] = ShaderManager.shaders[technique] or {}
   ShaderManager.shader_paths[technique] = string.sub(fullpath, 2)

   -- Check if an API version is in the shader key and prepend #version.
   local pos = 0
   local shadertype = Shader.type.vertex
   repeat
      dummy, pos, token = string.find(shaderKey, "([A-Za-z0-9]+)", pos + 1)
      if token and ShaderManager.ApiVersionToLanguageVersion[token] then
         local langVersion = ShaderManager.ApiVersionToLanguageVersion[token]
         shaderSource = "#version " .. langVersion .. "\n" .. shaderSource
      end
      if token and ShaderManager.StringToShaderType[token] then
         shadertype = ShaderManager.StringToShaderType[token]
      end
   until token == nil

   -- Add the shader
   if ShaderManager.shaders[technique][shaderKey] == nil then
      ShaderManager.shaders[technique][shaderKey] = Shader.allocate(shaderSource, shadertype)
   else
      ShaderManager.shaders[technique][shaderKey]:update(shaderSource)
   end
end

function ShaderManager.reloadeffect(technique)
   dofile(ShaderManager.shader_paths[technique])
   ShaderManager.geteffect(technique)
end

function ShaderManager.geteffect(technique)
   local shaders = {}
   if ShaderManager.shaders[technique] then
      for key,value in pairs(ShaderManager.shaders[technique]) do
         table.insert(shaders, value)
      end
   end

   if ShaderManager.effects[technique] == nil then
      ShaderManager.effects[technique] = ShaderProgram.allocate(shaders[1], shaders[2], shaders[3])
   else
      ShaderManager.effects[technique]:update(shaders[1], shaders[2], shaders[3])
   end

   return ShaderManager.effects[technique]
end
