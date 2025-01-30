#include "BSplineApproximateCurve.h"
#include <cmath>

void BSplineApproximateCurve::updatePoints() {
  // Update polygon
  if (points.size() == 0) {
    curvePolygon.setPoints({});
    curve.setPoints({});
    return;
  } else if (points.size() == 1) {
    curvePolygon.setPoints(points);
    curve.setPoints({});
    return;
  }
  curvePolygon.setPoints(points);

  if (points.size() < 4) {
    return;
  }

  const size_t TOTAL_SUBSEGMENTS = numSegments() * NUM_CURVE_SUBSEGMENTS;
  std::vector<glm::vec3> approximatedPoints(TOTAL_SUBSEGMENTS);
  float subsegment = 1.0f/TOTAL_SUBSEGMENTS;
  for (size_t i = 0; i < TOTAL_SUBSEGMENTS; i++) {
      approximatedPoints[i] = getCurvePoint(subsegment * i);
  }

  curve.setPoints(approximatedPoints);
}

void BSplineApproximateCurve::addPoint(glm::vec3 point) {
  if (points.size() == MAX_CURVE_POINTS) {
    points.clear();
  } else {
    points.push_back(point);
  }
  updatePoints();
}

void BSplineApproximateCurve::clearPoints() {
  points.clear();
  updatePoints();
}

glm::vec3 BSplineApproximateCurve::getCurvePoint(float t) {
  assert(t >= 0.0f && t <= 1.0f);
  assert(points.size() >= 4);

  int segment = getSegment(t);

  glm::mat4 controlPoints = glm::mat4(
    glm::vec4(points[segment], 1.0f),
    glm::vec4(points[segment+1], 1.0f),
    glm::vec4(points[segment+2], 1.0f),
    glm::vec4(points[segment+3], 1.0f));

  t = getSegmentParam(t);

  glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1);
  glm::mat4 B = glm::mat4 (
    -1.0f,  3.0f, -3.0f, 1.0f,
     3.0f, -6.0f,  0.0f, 4.0f,
    -3.0f,  3.0f,  3.0f, 1.0f,
     1.0f,  0.0f,  0.0f, 0.0f
  );

  return (T * (1.0f/6.0f) * B) * glm::transpose(controlPoints); // transpose zato sto je glm naopako
}

glm::vec3 BSplineApproximateCurve::getFirstDerivativeVec(float t) {
  assert(t >= 0.0f && t <= 1.0f);
  assert(points.size() >= 4);

  int segment = getSegment(t);

  glm::mat4 controlPoints = glm::mat4(
    glm::vec4(points[segment], 1.0f),
    glm::vec4(points[segment+1], 1.0f),
    glm::vec4(points[segment+2], 1.0f),
    glm::vec4(points[segment+3], 1.0f));

  t = getSegmentParam(t);

  glm::vec3 T = glm::vec3(t*t, t, 1);
  glm::mat4x3 B = glm::mat4x3 (
    -1.0f,  2.0f, -1.0f,
     3.0f, -4.0f,  0.0f,
    -3.0f,  2.0f,  1.0f,
     1.0f,  0.0f,  0.0f
  );

  return (T * (1.0f/2.0f) * B) * glm::transpose(controlPoints); // transpose zato sto je glm naopako
}

glm::vec3 BSplineApproximateCurve::getSecondDerivativeVec(float t) {
  assert(t >= 0.0f && t <= 1.0f);
  assert(points.size() >= 4);

  int segment = getSegment(t);

  glm::mat4 controlPoints = glm::mat4(
    glm::vec4(points[segment], 1.0f),
    glm::vec4(points[segment+1], 1.0f),
    glm::vec4(points[segment+2], 1.0f),
    glm::vec4(points[segment+3], 1.0f));

  t = getSegmentParam(t);

  glm::vec3 T = glm::vec3(t*t, t, 1);
  glm::mat4x2 B = glm::mat4x2 (
    -1.0f,  1.0f,
     3.0f, -2.0f,
    -3.0f,  1.0f,
     1.0f,  0.0f
  );

  return (T * B) * glm::transpose(controlPoints); // transpose zato sto je glm naopako
}
