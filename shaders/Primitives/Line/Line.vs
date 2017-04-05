#version 330

layout(location=0) in vec4 inPosition;
layout(location=1) in vec4 inColor;

out vec4 outColor;

void main() {
  outColor = inColor;
  gl_Position = vec4(inPosition);
}
