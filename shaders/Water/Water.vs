#version 130

in vec4 vertex_position;

uniform sampler2D water;
uniform sampler2D ground;

out vec2 texCoords;
out vec3 position;
out vec4 shadowCoord;

uniform mat4 lightMVP;
uniform mat4 MVP;

uniform mat4 heightmapScaleMatrix;

uniform vec2 offset;

void main() {
  float y1 = texture(water, vertex_position.zw).x;
  float y2 = texture(ground, vertex_position.zw).x;
  vec2 newVP = vertex_position.xy + offset;
  vec4 pos = vec4(newVP.x, y1+y2, newVP.y, 1.0);
  vec4 inPositionScaled = heightmapScaleMatrix * pos;
  
  gl_Position = MVP*inPositionScaled;
  
  texCoords = vertex_position.zw;
  position = inPositionScaled.xyz;
  shadowCoord = lightMVP * inPositionScaled;
  shadowCoord.xyz /= shadowCoord.w;
  shadowCoord.xyz += 1.0;
  shadowCoord.xyz *= 0.5;
}