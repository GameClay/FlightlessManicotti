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

DeclareShader('Geometry.GL2', [[
   #extension GL_EXT_geometry_shader4 : enable

   void main()
   {
      int i;
      for(i = 0; i < gl_VerticesIn; i++)
      {
         float size = 0.03;

         // Bottom Left
         gl_Position = vec4(-1.0f, -1.0f, 0.0, 1.0) * size * 0.5 + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(0.0, 0.0, 0.0, 0.0);
         EmitVertex();

         // Top Left
         gl_Position = vec4(-1.0f,  1.0f, 0.0, 1.0) * size * 0.5 + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(0.0, 1.0, 0.0, 0.0);
         EmitVertex();

         // Bottom Right
         gl_Position = vec4( 1.0f, -1.0f, 0.0, 1.0) * size * 0.5 + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(1.0, 0.0, 0.0, 0.0);
         EmitVertex();
         EndPrimitive();

         // Top Left
         gl_Position = vec4(-1.0f,  1.0f, 0.0, 1.0) * size * 0.5 + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(0.0, 1.0, 0.0, 0.0);
         EmitVertex();

         // Bottom Right
         gl_Position = vec4( 1.0f, -1.0f, 0.0, 1.0) * size * 0.5 + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(1.0, 0.0, 0.0, 0.0);
         EmitVertex();

         // Top Right
         gl_Position = vec4( 1.0f,  1.0f, 0.0, 1.0) * size * 0.5 + gl_PositionIn[i];
         gl_TexCoord[0] = vec4(1.0, 1.0, 0.0, 0.0);
         EmitVertex();

         EndPrimitive();
      }
   }
]])