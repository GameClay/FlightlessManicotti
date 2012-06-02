require 'render.ShaderFactory'

DeclareShader('Vertex.GL3', [[
   in vec3 InPosition;
   in vec3 InNormal;
   in vec4 InColor;

   out vec4 color0;
   out vec4 vNormal;

   uniform mat4 object_to_screen;

   void main()
   {
      vec4 pos = vec4(InPosition, 1.0);
      gl_Position = object_to_screen * pos;
      color0 = InColor;
      vec4 nrm = pos + vec4(InNormal, 0.0) * 0.5;
      vNormal = object_to_screen * nrm;
   }
]])

DeclareShader('Geometry.GL3', [[
   layout(points) in;
   layout(line_strip, max_vertices = 2) out;

   in vec4 vNormal[];
   in vec4 color0[];

   out vec4 frag_color0;

   void main()
   {
      // Point
      gl_Position = gl_in[0].gl_Position;
      frag_color0 = vec4(1.0, 0.0, 0.0, 1.0);
      EmitVertex();

      // Normal
      gl_Position = vNormal[0];
      frag_color0 = vec4(0.0, 0.0, 1.0, 1.0);
      EmitVertex();

      EndPrimitive();
   }
]])

DeclareShader('Fragment.GL3', [[
   in vec4 frag_color0;
   out vec4 fragout0;

   void main()
   {
      fragout0 = frag_color0;
   }
]])
