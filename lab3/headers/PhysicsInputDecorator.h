#include "IMoveable.h"
#include "IMoveableTransform.h"
#include "PhysicsEngine.h"

class PhysicsInputDecorator : public IMoveable {
  IMoveableTransform& moveable;
  PhysicsEngine &physicsEngine;

public:
  PhysicsInputDecorator(IMoveableTransform &moveable, PhysicsEngine &physicsEngine)
      : moveable(moveable), physicsEngine(physicsEngine) {}

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
  }

  void mouseLeftClick() override {
    physicsEngine.addSphere(moveable.PositionVec() + moveable.FrontVec() * 0.2f, moveable.FrontVec() * 3.0f, 0.1, 1);
  }

  void update(float deltaTime);

  virtual ~PhysicsInputDecorator() { }
};
