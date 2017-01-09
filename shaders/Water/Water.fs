#version 130

uniform sampler2D shadowMap;
uniform sampler2D water;
uniform sampler2D ground;
uniform sampler2D flowMap;
uniform sampler2D normals;
uniform sampler2D normalMap;

in vec2 texCoords;
in vec3 position;
in vec4 shadowCoord;

uniform mat4 inverse_proj;
uniform mat3 inverse_rot;
uniform int shadowSamples;

uniform mat4 lightView;
uniform mat4 view;

uniform vec2 offset;

out vec4 out_color;

vec3 getWorldNormal(vec2 coord) {
  vec2 viewport = vec2(1600, 900);
  vec2 fragCoord = ((coord/viewport)-0.5)*2;
  vec4 devNormal = vec4(fragCoord, 0.0, 1.0);
  vec3 eyeNoormal = normalize((inverse_proj*devNormal).xyz);
  return normalize(inverse_rot*eyeNoormal);
}

const float bias = 0.00042;  // shadow acne bias
const float spread = 2000;   // shadow sample spread

const vec2 poisson[16] = vec2[] (
  vec2 (-0.94201624,  -0.39906216),
  vec2 ( 0.94558609,  -0.76890725),
  vec2 (-0.094184101, -0.92938870),
  vec2 ( 0.34495938,   0.29387760),
  vec2 (-0.91588581,   0.45771432), 
  vec2 (-0.81544232,  -0.87912464), 
  vec2 (-0.38277543,   0.27676845), 
  vec2 ( 0.97484398,   0.75648379), 
  vec2 ( 0.44323325,  -0.97511554), 
  vec2 ( 0.53742981,  -0.47373420), 
  vec2 (-0.26496911,  -0.41893023), 
  vec2 ( 0.79197514,   0.19090188), 
  vec2 (-0.24188840,   0.99706507), 
  vec2 (-0.81409955,   0.91437590), 
  vec2 ( 0.19984126,   0.78641367), 
  vec2 ( 0.14383161,  -0.14100790) 
);

float doShadow() {
  float visibility = 1.0;
  for(int i = 0; i < shadowSamples; i++) {
    vec2 sc = vec2(shadowCoord.xy + poisson[i]/spread);
    float sample = texture(shadowMap, sc).x;
    if(sample < shadowCoord.z - bias)
      visibility -= 1.0/shadowSamples;
  }
  //return visibility;
  return clamp(visibility, 0.3, 1.0);
}

struct SHC {
  vec3 L00,  L1M1, L10, L11,
       L2M2, L2M1, L20, L21, L22;
};

const SHC beach = SHC(
  vec3( 0.6841148,  0.6929004,  0.7069543),
  vec3( 0.3173355,  0.3694407,  0.4406839),
  vec3(-0.1747193, -0.1737154, -0.1657420),
  vec3(-0.4496467, -0.4155184, -0.3416573),
  vec3(-0.1690202, -0.1703022, -0.1525870),
  vec3(-0.0837808, -0.0940454, -0.1027518),
  vec3(-0.0319670, -0.0214051, -0.0147691),
  vec3( 0.1641816,  0.1377558,  0.1010403),
  vec3( 0.3697189,  0.3097930,  0.2029923)
);

vec3 shLight(vec3 normal, const SHC l) {
  const float C1 = 0.429043;
  const float C2 = 0.511664;
  const float C3 = 0.743125;
  const float C4 = 0.886227;
  const float C5 = 0.247708;
  
  vec3 r1 = C1 * l.L22 * (normal.x*normal.x - normal.y*normal.y);
  vec3 r2 = C3 * l.L20 * normal.z*normal.z;
  vec3 r3 = C4 * l.L00; // - 
  vec3 r4 = C5 * l.L20;
  vec3 r5 = 2.0 * C1 * l.L2M2 * normal.x * normal.y;
  vec3 r6 = 2.0 * C1 * l.L21 * normal.x * normal.z;
  vec3 r7 = 2.0 * C1 * l.L2M1 * normal.y * normal.z;
  vec3 r8 = 2.0 * C2 * l.L11 * normal.x;
  vec3 r9 = 2.0 * C2 * l.L1M1 * normal.y;
  vec3 r10 = 2.0 * C2 * l.L10 * normal.z;

  vec3 sum = r1+r2+r3-r4+r5+r6+r7+r8+r9+r10;

  return sum;
}

vec3 Kr = vec3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131); // air
vec3 absorb(float dist, vec3 color, float factor) {
  return color-color*pow(Kr, vec3(factor/dist));
}

vec3 getNormal() {
  float s = 1.0/256.0;
  vec2 off = vec2(0.0,0.0);

  vec2 centerTexCoord = texCoords;
  
  vec2 centerFlow = texture(flowMap, centerTexCoord).xy;
  vec2 rightFlow  = texture(flowMap, centerTexCoord + vec2(s, 0.0)).xy;
  vec2 topFlow    = texture(flowMap, centerTexCoord + vec2(0.0, s)).xy;
  vec2 topRFlow   = texture(flowMap, centerTexCoord + vec2(s,   s)).xy;

  vec3 center = normalize(texture(normalMap, (texCoords+offset) * 32.0 - centerFlow * 2.0).xyz*2.0-1.0);
  vec3 right  = normalize(texture(normalMap, (texCoords+offset) * 32.0 - rightFlow * 2.0).xyz*2.0-1.0);
  vec3 top    = normalize(texture(normalMap, (texCoords+offset) * 32.0 - topFlow * 2.0).xyz*2.0-1.0);
  vec3 topR   = normalize(texture(normalMap, (texCoords+offset) * 32.0 - topRFlow * 2.0).xyz*2.0-1.0);

  vec3 normal1 = center + right;
  vec3 normal2 = top + topR;
  //vec3 normal1 = mix(center, right, off.x/s);
  //vec3 normal2 = mix(top, topR, off.x/s);
  vec3 normal = normal1 + normal2;
  //vec3 normal = mix(normal1, normal2, off.y/s);
  return normalize(normal*vec3(1.0, 1.0, 1.0));
}

const vec3 fog_colour = vec3 (1.0);
const float fog_rad_min = 5.0;
const float fog_rad_max = 12.0;

vec3 fog (vec4 c) {
  vec3 pos = vec3 (view * vec4 (position, 1));

  float dist = length (-pos);
  float fac = clamp ((dist - fog_rad_min) / (fog_rad_max - fog_rad_min), 0, 1);
  return mix (c.rgb, fog_colour, fac);
}

//const vec3 deep = vec3(0.0, 51.0/255.0, 128.0/255.0)*0.5;
//const vec3 turb = vec3(42.0/255.0, 212.0/255.0, 255.0/255.0)*0.9;
const vec3 deep = vec3(0.25, 0.75, 1.0) * 0.5;
const vec3 turb = vec3(0.25, 0.75, 1.0) * 0.6;


void main() {
  
  vec3 w = texture(water, texCoords).xyz;

  float speedFactor = clamp(length(w.yz)/0.02, 0.0, 1.0);
  float depthFactor = clamp(w.x/0.001, 0.0, 0.8);
  float visibilityFactor = clamp(speedFactor+depthFactor, 0.0, 1.0);

  vec3 baseNormal = normalize(texture(normals, texCoords).xyz);
  vec3 tangent = normalize(cross(baseNormal, vec3(0.0, 0.0, 1.0)));
  vec3 bitTangent = normalize(cross(tangent, baseNormal));
  mat3 orthoBasis = mat3(tangent, baseNormal, bitTangent);
  vec3 detailNormal = orthoBasis * getNormal();
  vec3 normal = normalize(mix(baseNormal*0.5+detailNormal*20, detailNormal, speedFactor));
  normal = normalize(mix(normal, baseNormal, sqrt(clamp(w.x*133.33333, 0.0, 1.0))*0.75));

  vec3 lightdir = (lightView * vec4(0.0, 0.0, 1.0, 1.0)).xyz;
  
  //vec3 eyeNormal = getWorldNormal(gl_FragCoord.xy);
  vec3 eyeNormal = normalize (vec3 (view * vec4 (baseNormal, 0.0)));

  vec3 specNormal = reflect(eyeNormal, normalize(normal * vec3(1.0, 0.35, 1.0)));
  float lambert = pow(max(0.0, dot(specNormal, lightdir)), 1);
  float specular = lambert * 0.05; //pow(lambert, 20.0)*0.01;

  vec3 color = mix(turb, deep, clamp(w.x*133.33, 0.0, 1.0));
  color = mix(color, vec3(1.0), clamp(pow(speedFactor*2.0, 1.0), 0.0, 1.0));

  vec3 exident = color * mix(vec3(1.0), shLight(normal, beach), 0.75);
  float visibility = doShadow();
  //float d = length(position.xyz);
  //vec3 incident = exident*mix(0.45, 1.0, visibility) + specular*visibility + 0.1;
  vec3 incident = exident*(visibility) + exident*0.2;
  //incident += pow(Kr*d*0.7, vec3(2.0));
  incident = fog(vec4(incident*(visibilityFactor), 1.0));
  out_color = vec4(incident, visibilityFactor);
  


  /*
  vec3 w = texture(water, texCoords).xyz;

  float speedFactor = clamp(length(w.yz)*50, 0.0, 1.0);
  float depthFactor = clamp(w.x*2000, 0.0, 1.0);

  vec3 baseNormal = normalize(texture(normals, texCoords).xyz);
  vec3 tangent = normalize(cross(baseNormal, vec3(0.0, 0.0, 1.0)));
  vec3 bitTangent = normalize(cross(tangent, baseNormal));
  mat3 orthoBasis = mat3(tangent, baseNormal, bitTangent);
  vec3 detailNormal = orthoBasis * getNormal();
  vec3 normal = normalize(mix(baseNormal*0.5+detailNormal*20, detailNormal, speedFactor));
  normal = normalize(mix(normal, baseNormal, sqrt(clamp(w.x*133.33333, 0.0, 1.0))*0.75));

  vec3 lightdir = (light.view * vec4(0.0, 0.0, 1.0, 1.0)).xyz;
  vec3 eyeNormal = getWorldNormal(gl_FragCoord.xy);
  vec3 specNormal = reflect(eyeNormal, normalize(normal * vec3(1.0, 0.35, 1.0)));
  float lambert = pow(max(0.0, dot(specNormal, lightdir)), 0.5);
  float specular = pow(lambert, 20.0)*0.9;

  vec3 color = mix(turb, deep, clamp(w.x*133.33, 0.0, 1.0));
  color = mix(color, vec3(1.0), clamp(pow(speedFactor*2.0, 1.0), 0.0, 1.0));

  vec3 exident = color * mix(shLight(specNormal, beach), shLight(normal, beach), 0.75);
  float visibility = doShadow();
  float d = length(position.xyz);
  vec3 incident = exident*mix(0.45, 1.0, visibility) + specular*visibility;
  //incident += pow(Kr*d*0.7, vec3(2.0));
  incident = fog(vec4(incident, 1.0));
  out_color = vec4(incident, depthFactor);
  */
  
}