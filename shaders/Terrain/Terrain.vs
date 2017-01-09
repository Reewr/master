#version 130

in vec4 vertex_position;
in vec4 gridTex;

uniform sampler2D ground;

out vec3 worldPos;
out vec2 texCoords;
out vec3 normals;
out vec4 shadowCoord;
out vec3 worldMVPPos;

struct Light {
  mat4 view;
  mat4 proj;
  mat4 model;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 heightmapScaleMatrix;

uniform mat4 lightMVP;
uniform mat4 MVP;

uniform vec2 offset;

uniform Light light;

void main () {
  float y = texture(ground, vertex_position.zw).x;
  vec2 newVP = vertex_position.xy + offset;
  vec4 pos = vec4(newVP.x, y, newVP.y, 1.0);
	vec4 inPositionScaled = heightmapScaleMatrix*pos;
  vec4 mvpPos = (MVP * inPositionScaled);
	worldMVPPos = mvpPos.xyz;
  gl_Position = mvpPos;

	texCoords = vertex_position.zw;

	worldPos = inPositionScaled.xyz;

  shadowCoord = lightMVP * inPositionScaled;
  shadowCoord.xyz /= shadowCoord.w;
  shadowCoord.xyz += 1.0;
  shadowCoord.xyz *= 0.5;
}