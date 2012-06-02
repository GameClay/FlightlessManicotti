require 'render.ShaderFactory'

DeclareShader('Vertex.GL2', [[
   attribute vec3 InPosition;
   attribute vec2 InTex0;

   varying vec2 tex_coord0;
   uniform float hax_color;

   void main()
   {
      gl_Position = gl_ModelViewProjectionMatrix * vec4(InPosition, 1.0);
      tex_coord0 = InTex0.xy;
      gl_FrontColor = vec4(hax_color, hax_color, hax_color, 1.0);
   }
]])

DeclareShader('Fragment.GL2', [[
   uniform sampler2D tex0;
   varying vec2 tex_coord0;

   void main()
   {
      gl_FragColor = texture2D(tex0, tex_coord0) * gl_Color;
   }
]])
