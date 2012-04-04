require 'render.ShaderFactory'

DeclareShader('Vertex.GL2', [[
   attribute float in_X;
   attribute float in_Y;
   attribute float in_Z;
   void main()
   {
      gl_Position = vec4(in_X, in_Y, in_Z, 1.0);
   }
]])

DeclareShader('Fragment.GL2', [[
   void main()
   {
      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
   }
]])
