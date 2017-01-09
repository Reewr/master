#version 130

in vec2 Texcoord;

uniform sampler2D water;

uniform float rain;
uniform float evaporation;
uniform float deltaTime;

const ivec2 iTopL = ivec2(-1, -1);
const ivec2 iTopR = ivec2( 1, -1);
const ivec2 iMiddleL = ivec2(-1, 0);
const ivec2 iMiddleR = ivec2(0, -1);
const float mult = 0.003165375;

out vec4 out_color;

void main() {
  vec3 w = texture(water, Texcoord).xyz;
  float limit = 60*deltaTime;
  vec2 topL = textureOffset(water, Texcoord, iTopL).yz;
  vec2 topR = textureOffset(water, Texcoord, iTopR).yz;
  vec2 middleL = textureOffset(water, Texcoord, iMiddleL).yz;
  vec2 middleR = textureOffset(water, Texcoord, iMiddleR).yz;
  vec2 middle = w.yz;

  vec2 sum1 = topL + middleR * 1.4 + topR;
  vec2 sum2 = middleL * 1.4 + middle * 300.0 + topR * 1.4;
 
  float addWater = limit*0.00000005*rain;
  float totalWater = clamp((w.x+addWater)*mix(1.0, 0.9999995, evaporation), 0.0, 1.0);
  w.x = totalWater;
  w.yz = (sum1 + sum2 + sum1)*mult;
  out_color = vec4(w, 1.0);
}
