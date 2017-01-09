#version 130

in vec2 Texcoord;
out vec4 out_color;

uniform sampler2D direction;

void main() {
  vec4 t = texture(direction, vec2(Texcoord.x, 1));
  out_color = t;
}