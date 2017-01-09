#version 130

in vec2 position;
in vec2 texcoord;

uniform vec2 screenRes;

out vec2 Texcoord;

void main() {
  vec2 newPos = position;

  newPos.x =  (newPos.x - screenRes.x/2)/(screenRes.x/2);
  newPos.y = -(newPos.y - screenRes.y/2)/(screenRes.y/2);

  gl_Position = vec4(newPos, 0.0, 1.0);
  Texcoord = texcoord;
}
