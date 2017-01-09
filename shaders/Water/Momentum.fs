#version 130

in vec2 Texcoord;

uniform sampler2D current;
uniform sampler2D last;
uniform sampler2D ground;
uniform float delta;

const ivec2 middleR = ivec2(1, 0);
const ivec2 middleL = ivec2(-1, 0);
const ivec2 top     = ivec2(0, 1);
const ivec2 bottom  = ivec2(0,-1);

const vec3 mult = vec3(0.25);

out vec4 out_color;

void main() {
  float g1 = texture(ground, Texcoord).x;
  float l1 = texture(last, Texcoord).x;
  vec3 c1 = texture(current, Texcoord).xyz;

  float limit = 60*delta;
  
  float gc1 = g1+l1;

  float g2 = textureOffset(ground,  Texcoord, middleR).x;
  float c2 = textureOffset(current, Texcoord, middleR).x;
  float gc2 = g2+c2;

  float g3 = textureOffset(ground,  Texcoord, middleL).x;
  float c3 = textureOffset(current, Texcoord, middleL).x;
  float gc3 = g3+c3;

  float g4 = textureOffset(ground,  Texcoord, top).x;
  float c4 = textureOffset(current, Texcoord, top).x;
  float gc4 = g4+c4;

  float g5 = textureOffset(ground,  Texcoord, bottom).x;
  float c5 = textureOffset(current, Texcoord, bottom).x;
  float gc5 = g5+c5;

  float change1 = gc2-gc1;
  float change2 = gc3-gc1;
  float change3 = gc4-gc1;
  float change4 = gc5-gc1;
  float c1Fourth = (-c1.x*0.25)*limit;
  
  change1 = clamp(change1, c1Fourth, (c2*0.25)*limit);
  change2 = clamp(change2, c1Fourth, (c3*0.25)*limit);
  change3 = clamp(change3, c1Fourth, (c4*0.25)*limit);
  change4 = clamp(change4, c1Fourth, (c5*0.25)*limit);

  vec3 ch1 = vec3(change1, -middleR*change1);
  vec3 ch2 = vec3(change2, -middleL*change2);
  vec3 ch3 = vec3(change3, -top*change3);
  vec3 ch4 = vec3(change4, -bottom*change4);

  vec3 v = (ch1+ch2+ch3+ch4)*mult;
  vec3 n = (c1 + v);
  out_color = vec4(n, 1.0);
}
