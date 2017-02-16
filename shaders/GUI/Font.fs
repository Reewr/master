#version 330

in vec2 Texcoord;
in vec4 Colors;

out vec4 outColor;

uniform sampler2D inTexture;
uniform bool isBackground;

uniform bool isColorOverriden;
uniform vec3 overrideColor;

void main() {
  vec4 texColor = texture(inTexture, Texcoord);
  vec4 usedColors = isColorOverriden ? vec4(overrideColor, 1.0) : Colors;
  outColor = isBackground ? Colors : texColor.r * usedColors;
}
