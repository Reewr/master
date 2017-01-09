#version 130

in vec2 Texcoord;

uniform sampler2D toCopy;

out vec4 out_color;

void main() {
  out_color = texture(toCopy, Texcoord);
}