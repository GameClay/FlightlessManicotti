require 'render.ShaderFactory'

DeclareShader('Vertex.GL3', [[
   in vec3 InPosition;
   in vec4 InColor;

   varying vec4 vNormal;

   uniform mat4 object_to_screen;

   void main()
   {
      vec4 pos = vec4(InPosition, 1.0);
      gl_Position = object_to_screen * pos;
      gl_FrontColor = gl_Color;
      vec4 nrm = pos + vec4(gl_Normal, 0.0) * 0.5;
      vNormal = object_to_screen * nrm;
   }
]])

DeclareShader('Geometry.GL3', [[
   #extension GL_EXT_geometry_shader4 : enable

   in vec4 vNormal[1]; // In = points so size = 1

   void main()
   {
      for(int i = 0; i < gl_VerticesIn; i++)
      {
         // Bottom Left
         gl_Position = gl_PositionIn[i];
         gl_FrontColor = vec4(1.0, 0.0, 0.0, 1.0);
         EmitVertex();

         // Top Left
         gl_Position = vNormal[0];
         gl_FrontColor = vec4(0.0, 0.0, 1.0, 1.0);
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
