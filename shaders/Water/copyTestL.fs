#version 130

in vec2 Texcoord;
out vec4 out_color;

uniform sampler2D waterNow;
uniform sampler2D waterDir;
uniform sampler2D ground;
uniform sampler2D groundDir;

uniform vec2 screenRes;

void main() {
  vec4 g1 = texture(ground, Texcoord);
  vec4 g2 = texture(groundDir, vec2(1.0-Texcoord.x, Texcoord.y));
  vec3 w1 = texture(waterNow, Texcoord).xyz;
  vec3 w2 = texture(waterDir, vec2(1.0-Texcoord.x, Texcoord.y)).xyz;

  float f1 = w1.x + g1.x;
  float f2 = w2.x + g2.x;

  float diff1 = clamp((f2-f1)*0.325, -w1.x*0.5, w2.x*0.5);

  vec3 v1 = vec3(diff1, -vec2(-1, 0)*diff1);
  out_color = (Texcoord.x == 0.5/screenRes.x) ? vec4(w1+v1, 1.0) : vec4(w1, 1.0);
}