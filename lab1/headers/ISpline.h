#pragma once

#include <glm/glm.hpp>

class ISpline {
public:
  virtual glm::vec3 getCurvePoint(float t) = 0;
  virtual glm::vec3 getFirstDerivativeVec(float t) = 0;
  virtual glm::vec3 getSecondDerivativeVec(float t) = 0;
  virtual ~ISpline() {}
};
