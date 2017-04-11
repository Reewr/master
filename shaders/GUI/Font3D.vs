#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 colors;

out vec2 Texcoord;
out vec4 Colors;

uniform mat4 MVP;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec2 size;
uniform vec3 worldPosition;

const vec2 halfScreenRes = _CFG_.Graphics.resolution;

void main() {
  Texcoord = texcoord;
  Colors = colors;

  vec2 normalizedPosition = position / halfScreenRes;

  vec3 vertexPosition = worldPosition +
    cameraRight * normalizedPosition.x +
    cameraUp * normalizedPosition.y * -1;

  gl_Position = MVP * vec4(vertexPosition, 1.0);
}
