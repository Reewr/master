#version 130

in vec2 Texcoord;
out vec4 out_color;

uniform sampler2D shadowMap;
uniform vec2 axis;

void main() {
  vec4 a = texture(shadowMap, Texcoord)*0.375;
  vec4 b = texture(shadowMap, Texcoord+axis)*0.3125;
  vec4 c = texture(shadowMap, Texcoord-axis)*0.3125;
  out_color = a+b+c;
}