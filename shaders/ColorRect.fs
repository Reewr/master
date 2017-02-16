#version 330

in vec2 Texcoord;
out vec4 outColor;

uniform vec4 guiColor;

void main() {
  outColor = guiColor;
}
