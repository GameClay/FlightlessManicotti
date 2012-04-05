require 'render.ShaderFactory'

DeclareShader('Vertex.GL2', [[
   attribute float in_X;
   attribute float in_Y;
   attribute float in_Z;

   varying vec4 size;

   void main()
   {
      gl_Position = vec4(in_X, in_Y, in_Z, 1.0);

      size = vec4(0.015, 0.015, 0.015, 1.0);
   }
]])

DeclareShader('Geometry.GL2', [[
   #extension GL_EXT_geometry_shader4 : enable

   varying in vec4 size[1]; // In = points so size = 1

   void main()
   {
      for(int i = 0; i < gl_VerticesIn; i++)
      {
         // Bottom Left
         gl_Position = vec4(-1.0f, -1.0f, 0.0, 1.0) * size[i] + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(0.0, 0.0, 0.0, 0.0);
         EmitVertex();

         // Top Left
         gl_Position = vec4(-1.0f, 1.0f, 0.0, 1.0) * size[i] + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(0.0, 1.0, 0.0, 0.0);
         EmitVertex();

         // Bottom Right
         gl_Position = vec4(1.0f, -1.0f, 0.0, 1.0) * size[i] + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(1.0, 0.0, 0.0, 0.0);
         EmitVertex();

         // Top Right
         gl_Position = vec4(1.0f, 1.0f, 0.0, 1.0) * size[i] + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(1.0, 1.0, 0.0, 0.0);
         EmitVertex();

         EndPrimitive();
      }
   }
]])

DeclareShader('Fragment.GL2', [[
   void main()
   {
      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
   }
]])
