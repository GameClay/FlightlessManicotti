require 'render.ShaderManager'

ShaderManager.declareshader('Vertex.GL3', [[
   in vec3 InPosition;
   in vec2 InTex0;

   out vec2 tex_coord0;

   void main()
   {
      gl_Position = vec4(InPosition, 1.0);
      tex_coord0 = InTex0.xy;
   }
]])

ShaderManager.declareshader('Fragment.GL3', [[
   in vec2 tex_coord0;

   out vec4 fragout0;

   uniform sampler2D tex0;
   uniform sampler2D tex1;

   uniform vec4 color0;
   uniform vec4 color1;

   void main()
   {
      fragout0 = texture(tex0, tex_coord0) * color0 + texture(tex1, tex_coord0) * color1;
   }
]])
