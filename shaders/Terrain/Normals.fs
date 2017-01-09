#version 130

in vec2 Texcoord;
out vec4 out_color;

uniform sampler2D ground;

struct SquareRow {
  vec2 L;
  vec2 M;
  vec2 R;
};

uniform SquareRow top;
uniform SquareRow middle;
uniform SquareRow bot;

vec3 getHeight(vec2 off) {
  vec2 tex = Texcoord+off;
  float h = texture(ground, tex).x;
  return vec3(tex.x, h, tex.y);
}

vec3 getNormal(vec3 pos, vec2 off) {
  vec3 v = getHeight(off) - pos;
  vec3 p = cross(vec3(0.0, 1.0, 0.0), v);
  return normalize(cross(v, p)); 
}

void main() {
  vec3 pos = getHeight(middle.M);
  
  vec3 normTopL = getNormal(pos, top.L);
  vec3 normTopM = getNormal(pos, top.M);
  vec3 normTopR = getNormal(pos, top.R);

  vec3 normMidL = getNormal(pos, middle.L);
  vec3 normMidR = getNormal(pos, middle.R);

  vec3 normBotL = getNormal(pos, bot.L);
  vec3 normBotM = getNormal(pos, bot.M);
  vec3 normBotR = getNormal(pos, bot.R);

  vec3 sumTop = normTopL + normTopM + normTopR;
  vec3 sumMid = normMidL + normMidR;
  vec3 sumBot = normBotL + normBotM + normBotR;

  vec3 normSum = (sumTop + sumMid + sumBot)*0.125;
  normSum = normalize(normSum);

  out_color = vec4(normSum, 1.0);
}