#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 colors;

out vec2 Texcoord;
out vec4 Colors;

const vec2 screenRes = _CFG_.Graphics.resolution;

uniform vec2 guiOffset;

float convertCoords(float x, float s) {
  return ((x - s/2)/(s/2));
}

void main() {
  Texcoord = texcoord;
  Colors = colors;
  vec2 newPos = position + guiOffset;

  newPos.x =  convertCoords(newPos.x, screenRes.x);
  newPos.y = -convertCoords(newPos.y, screenRes.y);

  gl_Position = vec4(newPos, 0.0, 1.0);
}
