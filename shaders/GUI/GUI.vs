#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;

out vec2 Texcoord;

const vec2 screenRes = _CFG_.Graphics.resolution;
uniform vec2 guiOffset;

float convertCoords(float x, float s) {
  return ((x - s/2)/(s/2));
}

void main() {
  Texcoord = texcoord;
  vec2 newPos = position + guiOffset;

  newPos.x =  convertCoords(newPos.x, screenRes.x);
  newPos.y = -convertCoords(newPos.y, screenRes.y);

  gl_Position = vec4(newPos, 0.0, 1.0);
}

