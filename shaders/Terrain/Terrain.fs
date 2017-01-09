#version 130

in vec3 worldPos;
in vec2 texCoords;
in vec4 shadowCoord;
in vec3 worldMVPPos;

uniform sampler2D shadowMap;
uniform sampler2D ground;
uniform sampler2D water;
uniform sampler2D normals;
//Textures
uniform sampler2D rockTex;
uniform sampler2D rockNormalTex;

uniform sampler2D grassTex;
uniform sampler2D grass2Tex;
uniform sampler2D grassNormalTex;

uniform sampler2D mudTex;
uniform sampler2D mudNormalTex;

uniform int shadowSamples;
uniform vec2 offset;
uniform mat4 view;
uniform vec3 lightDir;

const float bias = 0.00042;  // shadow acne bias (0.001)
const float spread = 500;   // shadow sample spread

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

out vec4 outputColor;

// fog
const vec3 fog_colour = vec3 (1.0);
const float fog_rad_min = 5.0;
const float fog_rad_max = 12.0;

vec3 fog (vec3 c) {
  vec3 position = vec3 (view * vec4 (worldPos, 1));
  float dist = length (-position);
  float fac = clamp ((dist - fog_rad_min) / (fog_rad_max - fog_rad_min), 0, 1);
  return mix(c.rgb, fog_colour, fac);
}

float getShadow(int i) {
  float sample = texture(shadowMap, vec2(shadowCoord.xy + poisson[i]/spread)).x;
  return step(sample, shadowCoord.z - bias);
}

float getVisibility() {
  float visibility = 1.0;
  float divSS = 1.0/shadowSamples;
  for(int i = 0; i < shadowSamples; i++)
    visibility -= getShadow(i)*divSS;
  return clamp(visibility, 0.3, 1.0);
}

vec3 addBumpmap(sampler2D t, sampler2D n, vec3 lightPos, vec2 uvs) {
  vec3 normal = normalize(texture(n, uvs).xyz*2.0-1.0);
  vec3 tex = texture(t, uvs).xyz;
  float diffuse = max(dot(normal, lightPos), 0.0);
  return tex * diffuse;
}

void main() {

  /*
  vec3 w = texture2D(water, texCoords).yxz;
  vec4 g = texture(ground, texCoords);
  float mixVal  = clamp(w.x*500, 0.0, 1.0);
  vec2 dx = dFdx(newCoords*32.0);
  vec2 dy = dFdy(newCoords*32.0);
  vec3 stone2Color = textureGrad(rockTex, 32*newCoords, dx, dy).rgb;
  vec3 stoneColor = textureGrad(rockNormalTex, 32.0*newCoords, dx, dy).rgb;
  stoneColor = stoneColor*0.3 + stone2Color*0.7;
  vec3 grassColor = texture(grassTex, 32.0*newCoords).rgb;
  vec3 dirtColor = texture(mud,  32.0*newCoords).rgb;
  vec3 soilColor = grassColor;

  */
  vec3 w = texture(water, texCoords).xyz;
  vec4 g = texture(ground, texCoords);
  float mixVal  = clamp(w.x*500.0, 0.0, 1.0);
  float mixVal2 = clamp(g.z*500.0, 0.0, 1.0);
  //float mixVal2 = clamp(g.z*500.0, 0.0, 1.0);
  vec2 newCoords = texCoords + offset;
  //vec3 lightPos = normalize(vec3(1.0, 1.0, 1.5));
  vec3 lightPos = normalize(worldMVPPos*30 - lightDir);
  float visibility = getVisibility();
  vec3 mapNormal = normalize(texture(normals, texCoords).xyz);
  vec3 tangent = normalize(cross(mapNormal, vec3(0.0, 0.0, 1.0)));

  vec3 grassColor = addBumpmap(grassTex,   grassNormalTex, lightPos, 64.0*newCoords);
  vec3 grass2Color = addBumpmap(grass2Tex, grassNormalTex, lightPos, 64.0*newCoords);
  //vec3 mudColor = addBumpmap(mudTex, mudNormalTex, lightPos, 64.0*newCoords);
  
  //vec3 dirt = vec3(85.0/255.0, 53.0/255.0, 0.0);
  grassColor = mix(grass2Color, grassColor, sqrt(clamp(w.x/0.0003+length(w.yz)/0.015, 0.0, 1.0)));
  //grassColor = mix(grassColor, grass2Color, 0.9);
  
  vec3 rockColor = addBumpmap(rockTex, rockNormalTex, lightPos, 64.0*newCoords);
  vec3 color = mix(rockColor, grassColor, mixVal2).xyz;
  
  vec3 finalColor = color;// = mix(grassColor, rockColor, clamp(g.x*50, 0.0, 1.0));
  //texel.rgb = fog(texel);
	//outputColor = texel; 
  finalColor = mix(grassColor, rockColor, abs(normalize(tangent)).y*2);
  //finalColor = mix(finalColor, grassColor, clamp(w.x*133, 0.0, 1.0));
  //finalColor = mix(finalColor, rockColor, clamp(g.x*100, 0.0, 1.0));
  //finalColor = mix(finalColor, mudColor, mixVal);
  finalColor = fog(finalColor * visibility + finalColor*0.2);
  outputColor = vec4(finalColor, 1.0);

}