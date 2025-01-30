#pragma once

#include "BezierCurve.h"
#include "IMoveableTransform.h"

class BezierManagerDecorator : public IMoveable {
  IMoveableTransform& moveable;
  BezierCurve &bezierCurves;
  bool shouldAnimate;
  float t;
  float animationSpeed;

public:
  BezierManagerDecorator(IMoveableTransform &moveable, BezierCurve &bezierCurves)
      : moveable(moveable), bezierCurves(bezierCurves), shouldAnimate(false), animationSpeed(0.1) {}

  void onMouseScroll(double yoffset) override {
    moveable.onMouseScroll(yoffset);
  }

  void onMouseMove(double xpos, double ypos) override {
    moveable.onMouseMove(xpos, ypos);
  }

  void processKeyboard(MoveableMovement movement, float deltaTime) override {
    moveable.processKeyboard(movement, deltaTime);
  }

  void mouseRightClick() override {
    startAnimation();
  }

  void mouseLeftClick() override {
    glm::vec3 position = moveable.PositionVec() + moveable.FrontVec() / 2.f;
    bezierCurves.addPoint({position.x, position.y, position.z});
  }

  void update(float deltaTime);
  void startAnimation();

  virtual ~BezierManagerDecorator() { }
};
