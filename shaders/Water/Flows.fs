#version 130

in vec2 Texcoord;

uniform sampler2D water;
uniform sampler2D flows;

out vec4 out_color;

void main() {
  vec2 pos = texture(flows, Texcoord).xy;
  vec3 w = texture(water, Texcoord).xyz;
  vec2 vel = (w.yz*0.001)/(w.x*0.1+0.001);

  out_color = vec4(pos+vel, 0.0, 1.0);
}