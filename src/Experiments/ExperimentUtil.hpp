#pragma once

#include <mmm.hpp>

class btQuaternion;
class btVector3;

// This file contains functions that are useful while doing experiments

namespace ExpUtil {
  // Calculates a score based on the `zeroIsBest` value. The closer
  // the value is to 0, the better. Range [0, 1]
  float score(float deltaTime, float zeroIsBest, float bias = 0.05f);

  // Turns a quaternion into rotation angles in radians
  mmm::vec3 getEulerAngles(float x, float y, float z, float w);
  mmm::vec3 getEulerAngles(const btQuaternion& q);

  // Turns a btVector3 to mmm::vec3
  mmm::vec3 fromVector(const btVector3& v);

  // normalizes an angle between low and up where rest is considered 0
  float normalizeAngle(float angle, float low, float up, float rest);

  // Opposite normalize
  float denormalizeAngle(float p, float low, float up, float rest);
}
