/**
 * @brief
 *
 * This file describes `fog` functions that can be used to
 * apply fog to elements that are nearing the far-plane and will
 * therefore be clipped.
 *
 * This is to simulate the horizon.
 *
 * To use: #include "lib/fog.glsl"
 */

/**
 * @brief
 *   Send the position of the vertex and the current color.
 *   It will use the length of the position and some internal variables
 *   to determine how it should mix the fog color with the current color
 *
 * @param pos
 * @param currentColor
 *
 * @return
 */
vec3 addFog(vec3 pos, vec4 currentColor);

// Implementation details
// ----------------------------------------------------------

const vec3  FOG_COLOR    = vec3(1.0);
const float FOG_RAD_MIN  = 32.0;
const float FOG_RAD_MAX  = 64.0;
const float FOG_RAD_DIFF = FOG_RAD_MAX - FOG_RAD_MIN;

vec3 addFog(vec3 pos, vec4 currentColor) {
  float dist   = length(-pos);
  float factor = clamp((dist - FOG_RAD_MIN) / FOG_RAD_DIFF, 0.0, 1.0);

  return mix(currentColor.rgb, FOG_COLOR, factor);
}
