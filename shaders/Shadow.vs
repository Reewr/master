#version 130

in vec3 vpos;

struct Light {
  mat4 view;
  mat4 proj;
  mat4 model;
};

uniform mat4 model;
uniform Light light;

void main () {
  gl_Position = light.proj * light.view * model * vec4 (vpos, 1.0);
}