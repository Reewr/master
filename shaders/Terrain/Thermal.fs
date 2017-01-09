#version 130

in vec2 Texcoord;

uniform sampler2D ground;
uniform vec2 screenRes;

const float tallus = 1.0/1024.0;
const ivec2 top = ivec2(0, -1);
const ivec2 bot = ivec2(0, 1);
const ivec2 left = ivec2(-1, 0);
const ivec2 right = ivec2(1, 0);

out vec4 output_color;

float thermal(vec2 pos) {
  vec2 texUp = vec2(0, 1.0/screenRes.y);
  vec2 texLeft = vec2(1.0/screenRes.x, 0); 

  float current = texture(ground, pos).y;
  float top     = current - textureOffset(ground, pos, top).y;
  float left    = current - textureOffset(ground, pos, left).y;
  float right   = current - textureOffset(ground, pos, right).y;
  float bottom  = current - textureOffset(ground, pos, bot).y;

  float dl = abs(max(current-left, 0.0));
  float db = abs(max(current-bottom, 0.0));

  float dt = abs(max(current-top, 0.0));
  float dr = abs(max(current-right, 0.0));
  float avg = (current+top+left+right+bottom)*0.2;
  float dm = max(dl, max(db, max(dt, dr)));
  current = current*step(dm, tallus) + avg*step(tallus, dm);
  return current;
}
float thermal2(vec2 pos) {
  const float maxD = 12.0/4096.0;
  float h = texture(ground, pos).y;

  float hl = textureOffset(ground, pos, left).y;
  float hr = textureOffset(ground, pos, right).y;
  float ht = textureOffset(ground, pos, top).y;
  float hb = textureOffset(ground, pos, bot).y;

  float dl = h - hl;
  float dr = h - hr;

  float dt = h - ht;
  float db = h - hb;

  float temp = h;

  if ((abs(dl) <= maxD || abs(dr) <= maxD))
  {
      float avg = (h+hl+hr+ht+hb)/5.0;
      temp = avg;
  }
  else if ((abs(dt) <= maxD || abs(db) <= maxD))
  {
      float avg = (h+hl+hr+ht+hb)/5.0;
      temp = avg;
  }
  return temp; 
}

void main() {
  float thermalVal = thermal2(Texcoord);
  output_color = vec4(1.0);
}