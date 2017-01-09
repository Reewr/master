#version 130

uniform sampler2D direction;

in vec2 Texcoord;
out vec4 out_color;

void main() {
  out_color = texture(direction, vec2(0, Texcoord.y));
}