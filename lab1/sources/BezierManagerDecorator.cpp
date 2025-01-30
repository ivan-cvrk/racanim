#include "BezierManagerDecorator.h"

void BezierManagerDecorator::update(float deltaTime) {
  if (!shouldAnimate) return;

  t += deltaTime * animationSpeed;
  if (t > 1.0f) {
    t = 1.0f;
  }
  glm::vec3 point = bezierCurves.getInterpolated(1.0f - t);
  moveable.setPosition({point.x, point.y, point.z});

  if (t == 1.0f) {
    shouldAnimate = false;
  }
}

void BezierManagerDecorator::startAnimation() {
  t = 0.f;
  this->shouldAnimate = true;
}
