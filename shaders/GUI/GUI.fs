#version 330

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D inTexture;

uniform vec3 guiColor;
uniform bool isText;

void main() {
  vec4 texColor = texture(inTexture, Texcoord);
  texColor = (isText) ? (texColor.r * vec4(guiColor, 1.0)) : texColor;
  outColor = texColor;
}
