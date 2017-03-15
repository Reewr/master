#version 330

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec2 vertexTexCoord;
layout(location=2) in vec3 vertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

struct Light {
  mat4 view;
  mat4 proj;
  mat4 model;
};

uniform Light light;

out vec3 position;
out vec3 normal;
out vec4 shadowCoord;
out vec2 texCoord;

void main () {

  position = vec3(view * model * vec4(vertexPosition, 1.0));
  normal   = normalize (vec3(view * model * vec4(vertexNormal, 0.0)));
  texCoord = vertexTexCoord;

  gl_Position = proj * vec4(position, 1.0);

  shadowCoord = light.proj * light.view * model * vec4(vertexPosition, 1.0);
  shadowCoord.xyz /= shadowCoord.w;
  shadowCoord.xyz += 1.0;
  shadowCoord.xyz *= 0.5;
}
