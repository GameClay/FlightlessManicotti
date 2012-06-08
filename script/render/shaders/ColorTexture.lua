require 'render.ShaderFactory'

DeclareShader('Vertex.GL3', [[
   in vec3 InPosition;
   in vec2 InTex0;

   out vec2 tex_coord0;
   out vec4 color0;

   uniform vec4 color;
   uniform mat4 object_to_screen;

   void main()
   {
      gl_Position = object_to_screen * vec4(InPosition, 1.0);
      tex_coord0 = InTex0.xy;
      color0 = color;
   }
]])

DeclareShader('Fragment.GL3', [[
   in vec2 tex_coord0;
   in vec4 color0;

   out vec4 fragout0;

   uniform sampler2D tex0;

   void main()
   {
      fragout0 = texture(tex0, tex_coord0) * color0;
   }
]])
