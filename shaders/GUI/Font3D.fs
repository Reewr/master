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
  // If we are currently rending the background of the text,
  // just return the colors
  //
  // If we are instead rending the text and the color has been
  // overriden, ignore the color and just set the color
  // and sample from the texture to get the font
  if (isBackground)
    outColor = Colors;
  else {
    outColor  = isColorOverriden ? vec4(overrideColor, 1.0) : Colors;
    outColor *= texture(fontTexture, Texcoord).r;
  }
}
