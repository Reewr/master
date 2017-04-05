#version 330

layout(points) in;
layout(lines_strip, max_vertices = 64) out;

in vec3 vPosition[];
in vec4 vColor[];

out vec4 fColor;

const float PI = 3.1415926;

void main() {
  fColor = vColor[0];

  for(int i = 0; i < 64; i++) {
    float ang = PI * vPosition[0].z / 64.0 * i;

    vec4 offset = vec4(cos(ang), -sin(ang), 0.0, 0.0);
    gl_Position = gl_in[0].gl_position + offset;
    EmitVertex();
  }

  EndPrimitive();
}
