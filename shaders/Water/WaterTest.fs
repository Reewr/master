#version 130

uniform sampler2D water;
uniform sampler2D ground;
uniform sampler2D flowMap;
uniform sampler2D normalMap; // Water Normals - PRE-created;

uniform float frameTime;

in vec2 texCoords;
in vec3 position;

out vec4 out_color;

vec2 texScale = vec2(35.0, 20.0);

mat2 getMat(vec2 coord) {
  vec3 s = texture(flowMap, floor(coord)/texScale.x).rgb;
  vec2 flowDir = (s.xy - 0.5);
  return mat2(flowDir.x, -flowDir.y, flowDir.y, flowDir.x);
}

void main() {
  vec3 w = texture(water, texCoords).xyz;
  float depthFactor = clamp(w.x*1000, 0.0, 1.0);
  float speedFactor = clamp(length(w.yz), 0.0, 1.0);
  texScale.x = texScale.x * (1.0-depthFactor);
  vec2 flowCoord = texCoords * texScale.x;
  vec2 ff = abs(2*(fract(flowCoord)) - 1.0) - 0.5;
  ff = 0.5-4*ff*ff*ff;
  vec2 ffscale = sqrt(ff*ff + (1-ff)*(1-ff));
  vec2 tCoord = texCoords * texScale.y;
  vec2 offset = vec2(frameTime*0.01, 0);

  vec2 normalT0 = texture(normalMap, getMat(flowCoord)*tCoord - offset).rg;
  vec2 normalT1 = texture(normalMap, getMat(flowCoord+vec2(0.5, 0))*tCoord - offset*1.06+0.62).rg;
  vec2 normalTAB = ff.x * normalT0 + (1-ff.x) * normalT1;
  vec2 normalT2 = texture(normalMap, getMat(flowCoord+vec2(0, 0.5))*tCoord - offset*1.33+0.27).rg;
  vec2 normalT3 = texture(normalMap, getMat(flowCoord+vec2(0.5, 0.5))*tCoord - offset*1.24).rg;
  vec2 normalTCD = ff.x * normalT2 + (1-ff.x) * normalT3;
  vec2 normalT = ff.y * normalTAB + (1-ff.y) * normalTCD;
  normalT = (normalT - 0.5) / (ffscale.y * ffscale.x);
  normalT *= 0.3;

  vec3 finalNormal = vec3(normalT, sqrt(1-normalT.x*normalT.x - normalT.y*normalT.y));
  finalNormal = clamp(finalNormal, 0.0, 1.0);
  
  vec3 deep = vec3(0.0, 51.0/255.0, 128.0/255.0)*0.5;
  vec3 turbulent = vec3(42.0/255.0, 212.0/255.0, 255.0/255.0)*0.9;
  vec3 color = mix(turbulent, deep, sqrt(clamp(w.x/0.0075, 0.0, 1.0)));
  color = mix(color, vec3(1.0, 1.0, 1.0), clamp(pow(speedFactor*2.0, 3.0), 0.0, 1.0));
  out_color = vec4(color+finalNormal, depthFactor);

}