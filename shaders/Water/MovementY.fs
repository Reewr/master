#version 130

in vec2 Texcoord;

uniform sampler2D heights;
uniform sampler2D source;

out vec4 out_color;

const ivec2 axis1 = ivec2(0, 1);
const ivec2 axis2 = ivec2(0, -1);

void main() {
  vec3 h = texture(source, Texcoord).xyz;
  
  float t1 = texture(heights, Texcoord).x;
  float h1 = h.x;

  float t2 = textureOffset(heights, Texcoord, axis1).x;
  float h2 = textureOffset(source, Texcoord, axis1).x;

  float t3 = textureOffset(heights, Texcoord, axis2).x;
  float h3 = textureOffset(source, Texcoord, axis2).x;

  float f1 = t1+h1;
  float f2 = t2+h2;
  float f3 = t3+h3;

  float diff1 = clamp((f2-f1)*0.325, -h1*0.5, h2*0.5);
  float diff2 = clamp((f3-f1)*0.325, -h1*0.5, h3*0.5);

  vec3 v1 = vec3(diff1, -axis1*diff1);
  vec3 v2 = vec3(diff2, -axis2*diff2);

  vec3 c = h + (v1 + v2);
  out_color = vec4(c, 1.0);
}