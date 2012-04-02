require 'render.ShaderFactory'

DeclareShader('Vertex.GL2', [[
   void main()
   {
      gl_Position = ftransform();
   }
]])

DeclareShader('Fragment.GL2', [[
   void main()
   {
      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
   }
]])
