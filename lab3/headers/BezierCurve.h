#pragma once

#include "Lines.h"
#include "IRenderable.h"
#include <glm/glm.hpp>

const int MAX_BEZIER_POINTS = 10;

class BezierCurve : public IRenderable {
  Lines polygonCurve;
  Lines approximatedCurve;
  Lines interpolatedCurve;

  std::vector<glm::vec3> points;

  void updatePoints();
public:
  BezierCurve() {
    polygonCurve.setColor(glm::vec3{0.0f, 0.2f, 0.8f});
    approximatedCurve.setColor(glm::vec3{0.8f, 0.2f, 0.0f});
    interpolatedCurve.setColor(glm::vec3{0.1f, 8.0f, 0.1f});
  }

  virtual void draw(const Shader* shader) override {
    polygonCurve.draw(shader);
    approximatedCurve.draw(shader);
    interpolatedCurve.draw(shader);
  }

  void addPoint(glm::vec3 point);
  void clearPoints();
  glm::vec3 getAproximated(float t);
  glm::vec3 getInterpolated(float t);
};
