#version 130

in vec2 Texcoord;

uniform sampler2D ground;
uniform sampler2D water;
uniform float delta;

const float tf = 0.0015;
const float tm = 0.5;

const ivec2 middleL = ivec2(-1, 0);
const ivec2 middleR = ivec2(1, 0);
const ivec2 bot = ivec2(0, -1);
const ivec2 top = ivec2(0, 1);

uniform float factor;
out vec4 output_color;

void main() {
  vec4 gc = texture(ground, Texcoord);
  vec3 wc = texture(water, Texcoord).xyz;

  float erroded = ((length(wc.yz)*0.000005)/(wc.x+0.001))*factor;
  erroded /= 1.0 + gc.z*300.0;
  erroded *= 10.0*delta;
  gc.y -= erroded;
  gc.z += erroded;

  vec3 wcMiddleL = textureOffset(water, Texcoord, middleL).xyz;
  vec3 wcMiddleR = textureOffset(water, Texcoord, middleR).xyz;
  vec3 wcBot = textureOffset(water, Texcoord, bot).xyz;
  vec3 wcTop = textureOffset(water, Texcoord, top).xyz;

  vec3 gcMiddleL = textureOffset(ground, Texcoord, middleL).xyz;
  vec3 gcMiddleR = textureOffset(ground, Texcoord, middleR).xyz;
  vec3 gcBot = textureOffset(ground, Texcoord, bot).xyz;
  vec3 gcTop = textureOffset(ground, Texcoord, top).xyz;

  float dirtC1 = min(abs(wc.y*tf/(wc.x+0.001)), tm);
  float dirtC2 = min(abs(wc.z*tf/(wc.x+0.001)), tm);

  float dirtL = clamp(wcMiddleL.y*tf/(wcMiddleL.x+0.001), 0.0, tm) * gcMiddleL.z;
  float dirtR = clamp(-wcMiddleR.y*tf/(wcMiddleR.x+0.001), 0.0, tm) * gcMiddleR.z;
  float dirtB = clamp(wcBot.z*tf/(wcBot.x+0.001), 0.0, tm) * gcBot.z;
  float dirtT = clamp(-wcTop.z*tf/(wcTop.x+0.001), 0.0, tm) * gcTop.z;
  
  float soilOff = (dirtC1 + dirtC2) * gc.z;
  float soilOn = dirtL + dirtB + dirtT + dirtR;

  gc.z += ((soilOn - soilOff)*10.0*delta)*factor;
  gc.z = max(0.0, gc.z);
  gc.x = gc.y+gc.z;

  output_color = gc;
}