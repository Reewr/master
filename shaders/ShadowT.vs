#version 130

in vec2 vpos;
in vec2 vtex;

out vec2 texcoord;

void main () {
  texcoord = vtex;
  gl_Position = vec4 (vpos, 0.0, 1.0);
}