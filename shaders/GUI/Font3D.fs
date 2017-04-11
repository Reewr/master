#version 330

in vec2 Texcoord;
in vec4 Colors;

out vec4 outColor;

// bound to one due to shadowmap being bound to two.
layout(binding=1) uniform sampler2D fontTexture;
uniform bool isBackground;

uniform bool isColorOverriden = false;
uniform vec3 overrideColor;

void main() {
  vec4 texColor = texture(fontTexture, Texcoord);
  vec4 usedColors = isColorOverriden ? vec4(overrideColor, 1.0) : Colors;
  outColor = isBackground ? Colors : texColor.r * usedColors;
}
