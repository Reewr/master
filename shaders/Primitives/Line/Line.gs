#version 330

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 vPosition[];
in vec4 vColor[];

out vec4 fColor;

void main() {
  fColor = vColor[0];

  gl_Position = gl_in[0].gl_Position + vec4(vPosition[0].xy, 0, 0);
  EmitVertex();

  gl_Position = gl_in[0].gl_Position + vec4(vPosition[0].zw, 0, 0);
  EmitVertex();

  EndPrimitive();
}
