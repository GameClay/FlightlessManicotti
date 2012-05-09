require 'render.ShaderFactory'

DeclareShader('Vertex.GL2', [[
   varying vec4 vNormal;

   void main()
   {
      gl_Position = ftransform();
      gl_FrontColor = gl_Color;
      vNormal = vec4(gl_Normal, 0.0);
   }
]])

DeclareShader('Geometry.GL2', [[
   #extension GL_EXT_geometry_shader4 : enable

   varying in vec4 vNormal[1]; // In = points so size = 1

   void main()
   {
      for(int i = 0; i < gl_VerticesIn; i++)
      {
         // Bottom Left
         gl_Position = gl_PositionIn[i];
         gl_FrontColor = vec4(1.0, 0.0, 0.0, 1.0);
         EmitVertex();

         // Top Left
         gl_Position = gl_PositionIn[i] + normalize(gl_PositionIn[i] * vNormal[0]) * 0.1;
         gl_FrontColor = vec4(0.0, 1.0, 0.0, 1.0);
         EmitVertex();

         EndPrimitive();
      }
   }
]])

DeclareShader('Fragment.GL2', [[
   void main()
   {
      gl_FragColor = gl_Color;
   }
]])
