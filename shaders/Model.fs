#version 420

in vec3 position;
in vec3 normal;
in vec4 shadowCoord;
in vec2 texCoord;

uniform vec3 dir;

uniform int shadowSamples;
layout(binding=0) uniform sampler2D shadowMap;
layout(binding=1) uniform sampler2D diffuseMap;

out vec4 fragment;

// some constants
//const float bias = 0.00042;  // shadow acne bias
const float spread = 500;   // shadow sample spread

const vec2 poisson[16] = vec2[] (
  vec2(-0.94201624, -0.39906216),
  vec2( 0.94558609, -0.76890725),
  vec2(-0.09418410, -0.92938870),
  vec2( 0.34495938,  0.29387760),
  vec2(-0.91588581,  0.45771432),
  vec2(-0.81544232, -0.87912464),
  vec2(-0.38277543,  0.27676845),
  vec2( 0.97484398,  0.75648379),
  vec2( 0.44323325, -0.97511554),
  vec2( 0.53742981, -0.47373420),
  vec2(-0.26496911, -0.41893023),
  vec2( 0.79197514,  0.19090188),
  vec2(-0.24188840,  0.99706507),
  vec2(-0.81409955,  0.91437590),
  vec2( 0.19984126,  0.78641367),
  vec2( 0.14383161, -0.14100790)
);

// light properties
const float spec_exp = 1;

const vec3 specular_light = vec3(1.0, 1.0, 1.0);
const vec3 diffuse_light  = vec3(0.7, 0.7, 0.7);
const vec3 ambient_light  = vec3(0.2, 0.2, 0.2);

// pseudo random number generator
float random(vec3 seed, int i) {
  vec4 seed4 = vec4(seed, i);
  float dotprod = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
  return fract(sin(dotprod) * 43758.5453);
}

// fog
const vec3 fog_colour = vec3(1.0);
const float fog_rad_min = 32.0;
const float fog_rad_max = 64.0;

vec3 fog (vec4 c) {
  float dist = length(-position);
  float fac = clamp((dist - fog_rad_min) / (fog_rad_max - fog_rad_min), 0, 1);
  return mix(c.rgb, fog_colour, fac);
}

void main () {

  // phong lighting
  vec3 texel = vec3(texture(diffuseMap, texCoord));

  vec3 ambient = ambient_light * texel;

  float dot_ = max(dot (dir, normal), 0);
  float theta = clamp(dot (dir, normal), 0, 1);
  vec3 diffuse = diffuse_light * texel * dot_;

  dot_ = max(dot(reflect(-dir, normal), normalize(-position)), 0);
  float specular_factor = pow(dot_, spec_exp);
  vec3 specular = specular_light * texel * specular_factor;

  // shadow
  float visibility = 1.0;

  float bias = 0.005 * tan(acos(theta));
  bias = clamp(bias, 0.0, 0.01);

  // poisson sampling
  for (int i = 0; i < shadowSamples; i++) {

    // fixed poison sampling
    int index = i;

    // stratified poisson sampling
    // int index = int (16.0 * random (floor (position * 1000), i)) % 16;

    vec2 sc = vec2(shadowCoord.xy + poisson[index]/spread);
    float sample_ = texture(shadowMap, sc).x;


    if (sample_ < shadowCoord.z - bias) {
      visibility -= 1.0/shadowSamples;
    }
  }

  if (dir.y < 0) {
    visibility += dir.y;
  }


  // ignore shadows:
  fragment = texture(diffuseMap, texCoord);

  // with shadows:
  // fragment = vec4((specular + diffuse) * visibility + ambient, 1.0);

  // with fog:
  // fragment.rgb = fog(fragment);
}
