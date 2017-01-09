#version 130

in vec3 vertex_position;
in vec2 vertex_texCoord;
in vec3 vertex_normal;

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
  
  position = vec3 (view * model * vec4 (vertex_position, 1.0));
  normal   = normalize (vec3 (view * model * vec4 (vertex_normal, 0.0)));
  texCoord = vertex_texCoord;

  gl_Position = proj * vec4 (position, 1.0);
  
  shadowCoord = light.proj * light.view * model * vec4 (vertex_position, 1.0);
  shadowCoord.xyz /= shadowCoord.w;
  shadowCoord.xyz += 1.0;
  shadowCoord.xyz *= 0.5;
}