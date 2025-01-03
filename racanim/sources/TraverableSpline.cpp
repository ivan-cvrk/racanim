#include "TraversableSpline.h"

void TraversableSpline::setPoint(float t) {
  position = spline->getCurvePoint(t);
  tangent = glm::normalize(spline->getFirstDerivativeVec(t));
  normal = glm::normalize(glm::cross(tangent, spline->getSecondDerivativeVec(t)));
  bitangent = glm::normalize(glm::cross(tangent, normal));

  tangentLine.setPoints({position, position + tangent});
  normalLine.setPoints({position, position + normal});
  bitangentLine.setPoints({position, position + bitangent});
}
