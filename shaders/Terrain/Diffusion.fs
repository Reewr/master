#version 130

in vec2 Texcoord;

uniform sampler2D ground;
uniform float delta;

const ivec2 middleR = ivec2(1, 0);
const ivec2 middleL = ivec2(-1, 0);
const ivec2 top = ivec2(0, 1);
const ivec2 bottom = ivec2(0, -1);

out vec4 out_color;

vec2 moveDirt(vec2 g1, vec2 g2) {
  float diff = (g2.x - g1.x)*0.25;
  float sdiff = clamp(diff, -g1.y*0.5, g2.y*0.5)*0.08;
  return (abs(diff) > 0.01) ? vec2(diff*0.1, sdiff) : vec2(0.0, sdiff);
}

void main() {
  vec4 g1 = texture(ground, Texcoord);
  vec4 g2 = textureOffset(ground, Texcoord, middleR);
  vec4 g3 = textureOffset(ground, Texcoord, middleL);
  vec4 g4 = textureOffset(ground, Texcoord, top);
  vec4 g5 = textureOffset(ground, Texcoord, bottom);

  vec2 c1 = moveDirt(g1.xz, g2.xz);
  vec2 c2 = moveDirt(g1.xz, g3.xz);
  vec2 c3 = moveDirt(g1.xz, g4.xz);
  vec2 c4 = moveDirt(g1.xz, g5.xz);

  vec2 sumc = (c1+c2+c3+c4);

  out_color = g1 + vec4(0.0, sumc, 0.0);
}