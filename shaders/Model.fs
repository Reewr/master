#version 330

in vec3 position;
in vec3 normal;
in vec4 shadowCoord;
in vec2 texCoord;

uniform vec3 dir;
uniform vec4 overrideColor = vec4(-1);
uniform bool useNormalsAsColors = false;

layout(binding=0) uniform sampler2D shadowMap;
layout(binding=1) uniform sampler2D diffuseMap;

out vec4 fragment;

#include "lib/shadow.glsl"

// light properties
const float spec_exp = 1;

const vec3 specular_light = vec3(1.0, 1.0, 1.0);
const vec3 diffuse_light  = vec3(0.7, 0.7, 0.7);
const vec3 ambient_light  = vec3(0.2, 0.2, 0.2);

void main () {

  // phong lighting
  vec3 texel;

  if (useNormalsAsColors)
    texel = normal;
  else if (overrideColor.x < 0)
    texel = vec3(texture(diffuseMap, texCoord));
  else
    texel = vec3(overrideColor.xyz);

  vec3 ambient = ambient_light * texel;

  float dot_ = max(dot (dir, normal), 0);
  float theta = clamp(dot (dir, normal), 0, 1);
  vec3 diffuse = diffuse_light * texel * dot_;

  dot_ = max(dot(reflect(-dir, normal), normalize(-position)), 0);
  float specular_factor = pow(dot_, spec_exp);
  vec3 specular = specular_light * texel * specular_factor;

  // shadow
  float visibility = fixedPoissonSampling(dir, normal, shadowCoord, shadowMap);

  // ignore shadows:
  //fragment = texture(diffuseMap, texCoord);
  fragment = vec4(texel, 1.0);

  // with shadows:
  // fragment = vec4((specular + diffuse) * visibility + ambient, 1.0);

  // without phong shading, until we fix normals...
  // fragment = vec4(texel * visibility + ambient, 1.0);

  // with fog:
  // fragment.rgb = fog(fragment);
}
