#version 130

in vec2 Texcoord;

uniform sampler2D ground;
uniform sampler2D water;

out vec4 out_color;

struct SquareRow {
  vec2 L;
  vec2 M;
  vec2 R;
};

uniform SquareRow top;
uniform SquareRow middle;
uniform SquareRow bot;

vec3 getHeight(vec2 offset) {
  vec2 tex = Texcoord+offset;
  float g = texture(ground, tex).x;
  float w = texture(water, tex).x;
  return vec3(tex.x, g+w, tex.y);
}

vec3 getNormal(vec3 pos, vec2 offset) {
  vec3 v = getHeight(offset) - pos;
  vec3 perp = cross(vec3(0.0, 1.0, 0.0), v);
  return normalize(cross(v, perp));
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