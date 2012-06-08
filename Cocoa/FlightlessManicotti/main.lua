-- Set up script library location
package.path = './script/?.lua;./script/?/init.lua;'..package.path

function dumptable(table)
   print(tostring(table))
   if table then
      for key,value in pairs(table) do
         print("\t"..key..": "..tostring(value))
      end
   end
end

require 'events'
require 'game.components'
require 'game.Lsystem'

-- Main is executed only once, it is not a loop. It receives the arguments
-- that were passed to the 'kl_initialize' function.
function main(...)
   -- Assign a handler for console command input
   Events.subscribe(consoleinput, Events.consoleinput);

   -- Sign up for the Init/Destroy events
   Events.subscribe(testinit, Events.init)
   Events.subscribe(testdestroy, Events.destroy)
   Events.subscribe(testrenderinit, Events.renderinit)
end

function consoleinput(_, inputstring)
   print("> "..inputstring)
   print(loadstring(inputstring)())
end

function testrenderinit()
   print("RenderInit called!")

   -- Create a render list with 32 entries
   script_render_list = RenderList.new(32)

   -- Full screen quad
   local verts = {{-1.0, -1.0, 0.0}, {-1.0, 1.01, 0.0}, {1.0, -1.0, 0.0}, {1.0, 1.0, 0.0}}
   local idxs = {0, 1, 2, 1, 3, 2}
   local texcoords = {{0, 0}, {0, 1}, {1, 0}, {1, 1}}
   fs_quad_mesh = Mesh.new()
   fs_quad_mesh:setpositions(verts)
   fs_quad_mesh:settexcoords(texcoords)
   fs_quad_mesh:setindices(idxs)
   fs_quad_mesh:update(Mesh.element.vertex + Mesh.element.index + Mesh.element.tex0, Mesh.element.none)

   -- Texture from file
   uv_template_texture = Texture.new[[uvtemplate001-lg.jpg]]

   -- Render instance for textured quad
   fs_quad_inst = RenderInstance.new()
   fs_quad_inst:setmesh(fs_quad_mesh)
   fs_quad_inst:setmaterial("ColorTexture")
   fs_quad_inst:setdrawtype(RenderInstance.drawtype.triangles)
   fs_quad_inst:setshaderconstants({
      tex0 = uv_template_texture,
      color = {{1, 1, 1, 1}}
   })
   script_render_list:insert(fs_quad_inst)

   --[[local testmesh = Mesh.new()
   testmesh:loadctm("./bunny.ctm")
   testmesh:computenormals()
   testmesh:update(Mesh.element.vertex + Mesh.element.normal + Mesh.element.index, Mesh.element.none)]]
end

function testinit()
   print("Init called!")
end

function testdestroy()
   print("Destroy called!")
end
