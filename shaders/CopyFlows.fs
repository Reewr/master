#version 130

in vec2 Texcoord;

uniform sampler2D inTexture;

out vec4 out_color;

void main() {
  vec2 flow = texture(inTexture, Texcoord).xy;
  float blue = abs(min(0, flow.x));
  float red  = max(0, flow.x);
  float green = max(0, flow.y);
  vec3 white = vec3(abs(min(0, flow.y)));

  vec3 color = vec3(red, green, blue) + white;
  out_color = vec4(color, 1.0);
}