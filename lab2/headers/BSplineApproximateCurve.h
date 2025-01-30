#pragma once

#include "IRenderable.h"
#include "ISpline.h"
#include "Lines.h"
#include <glm/glm.hpp>

const int MAX_CURVE_POINTS = 30;
const int NUM_CURVE_SUBSEGMENTS = 20;

class BSplineApproximateCurve : public IRenderable, public ISpline {
  Lines curvePolygon;
  Lines curve;

  std::vector<glm::vec3> points;

  void updatePoints();
  inline float getSegmentParam(float t) { return std::fmod(t, (1.0f / numSegments())) * numSegments(); }
  inline int getSegment(float t) { return glm::floor(t * numSegments()); }
  inline size_t numSegments() { return points.size() - 3; }

public:
  BSplineApproximateCurve() {
    curvePolygon.setColor(glm::ivec3(235, 213, 21));
    curve.setColor(glm::ivec3(121, 0, 242));
  }

  virtual void draw() override {
    curvePolygon.draw();
    curve.draw();
  }

  void showPoint(float t);
  void addPoint(glm::vec3 point);
  glm::vec3 getCurvePoint(float t) override;
  glm::vec3 getFirstDerivativeVec(float t) override;
  glm::vec3 getSecondDerivativeVec(float t) override;

  void clearPoints();
};
