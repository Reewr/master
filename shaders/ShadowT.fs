#version 130

in vec2 texcoord;

uniform sampler2D depth_tex;

out vec4 frag_colour;

void main () {
  frag_colour = texture (depth_tex, texcoord);
}