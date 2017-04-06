/**
 * @brief
 *
 * This file describes `random` functions that can be used to generate
 * psuedo random numbers or hashes. See the below functions for the content
 * and documentation.
 *
 * To use: #include "lib/random.glsl"
 */

/**
 * @brief
 *   This is not really an RNG since it will output
 *   the same number for the same inputs. This is a hashing
 *   function that depends on the implementation of sin on the GPU
 *   to generate errors in the computation and therefore give a more
 *   random number
 *
 * @param pSeed
 * @param pIndex
 *
 * @return
 */
float random(vec3 pSeed, int pIndex);

// Implementation details
// ----------------------------------------------------------

const vec4 RAND_VEC = vec4(12.9898, 78.233, 45.164, 94.673);

float random(vec3 pSeed, int pIndex) {
  vec4 seed4 = vec4(pSeed, pIndex);
  float dotprod = dot(seed4, RAND_VEC);
  return fract(sin(dotprod) * 43758.5453);
}
