/**
 * @brief
 *
 * This file describes `shadow` functions that can be used to
 * apply shadow from objects.
 *
 * To use: #include "lib/shadow.glsl"
 */
#include "../lib/random.glsl"

/**
 * @brief
 *   Performs Fixed Poisson Sampling using the given shadow map texture
 *   and shadow coordinates. Returns a visibility that can multiplied with
 *   the color to imitate shadow.
 *
 * @param pDirection   the direction the object is facing
 * @param pNormal      the normal of the vertex
 * @param pShadowCoord the shadow coordinate of the vertex
 * @param pShadowMap   the shadowmap texture itself
 *
 * @return
 */
float fixedPoissonSampling(vec3 pDirection,
                           vec3 pNormal,
                           vec4 pShadowCoord,
                           sampler2D pShadowMap);

/**
 * @brief
 *   Performs stratified Poisson Sampling. This process is slightly more
 *   performance heavy than fixed poisson sampling, but may give a better
 *   result.
 *
 *   Returns a visibility float that can be multiplied with the color
 *   to imitate shadow
 *
 * @param pDirection the direction the object is facing
 * @param pNormal    the normal of the vertex
 * @param pPosition  the position of the vertex
 * @param pShadowCoord the shadow coordinates of the vertex
 * @param pShadowMap   the shadowmap itself
 *
 * @return
 */
float stratifiedPoissonSampling(vec3 pDirection,
                                vec3 pNormal,
                                vec3 pPosition,
                                vec4 pShadowCoord,
                                sampler2D pShadowMap);

// Implementation details
// ----------------------------------------------------------
const int SHADOW_SAMPLES = _CFG_.Graphics.shadow_samples;
const float SHADOW_SAMPLE_DIFF = 1.0 / SHADOW_SAMPLES;

const float POISSON_BIAS = 0.00042;  // shadow acne bias
const float POISSON_SPREAD = 500;   // shadow sample spread
const vec2 POISSON[16] = vec2[] (
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


float fixedPoissonSampling(vec3 pDirection,
                           vec3 pNormal,
                           vec4 pShadowCoord,
                           sampler2D pShadowMap) {
  float visibility = 1.0;
  float theta = clamp(dot(pDirection, pNormal), 0.0, 1.0);
  float bias  = clamp(0.005 * tan(acos(theta)), 0.0, 0.01);

  for (int index = 0; index < SHADOW_SAMPLES; ++index) {
    vec2  sampleCoord  = pShadowCoord.xy + POISSON[index] / POISSON_SPREAD;
    float shadowSample = texture(pShadowMap, sampleCoord).x;

    if (shadowSample < pShadowCoord.z - bias)
      visibility -= SHADOW_SAMPLE_DIFF;
  }

  if (pDirection.y < 0)
    visibility += pDirection.y;

  return visibility;
}

float stratifiedPoissonSampling(vec3 pDirection,
                                vec3 pNormal,
                                vec3 pPosition,
                                vec4 pShadowCoord,
                                sampler2D pShadowMap) {
  float visibility = 1.0;
  float theta = clamp(dot(pDirection, pNormal), 0.0, 1.0);
  float bias  = clamp(0.005 * tan(acos(theta)), 0.0, 0.01);

  for (int i = 0; i < SHADOW_SAMPLES; ++i) {
    int   randomNum    = int (floor(random(pPosition * 1000, i)));
    int   index        = int (16.0 * randomNum) % 16;
    vec2  sampleCoord  = pShadowCoord.xy + POISSON[index] / POISSON_SPREAD;
    float shadowSample = texture(pShadowMap, sampleCoord).x;

    if (shadowSample < pShadowCoord.z - bias)
      visibility -= SHADOW_SAMPLE_DIFF;
  }

  if (pDirection.y < 0)
    visibility += pDirection.y;

  return visibility;
}
